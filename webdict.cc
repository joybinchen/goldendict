//
// Created by joybin on 2020/3/1.
//

/* This file is (c) 2008-2012 Konstantin Isakov <ikm@goldendict.org>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE file */

#include "forvo.hh"
#include "wstring_qt.hh"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtXml>
#include <list>
#include "audiolink.hh"
#include "htmlescape.hh"
#include "country.hh"
#include "language.hh"
#include "langcoder.hh"
#include "utf8.hh"
#include "gddebug.hh"
#include "qt4x5.hh"

namespace Forvo {

using namespace Dictionary;

void ForvoArticleRequest::cancel()
{
  finish();
}

ForvoArticleRequest::ForvoArticleRequest( wstring const & str,
                                          vector< wstring > const & alts,
                                          QString const & apiKey_,
                                          QString const & languageCode_,
                                          string const & dictionaryId_,
                                          QNetworkAccessManager & mgr )
{
  connect( &mgr, SIGNAL( finished( QNetworkReply * ) ),
           this, SLOT( requestFinished( QNetworkReply * ) ),
           Qt::QueuedConnection );
  
  addQuery(  mgr, str );

  for( unsigned x = 0; x < alts.size(); ++x )
    addQuery( mgr, alts[ x ] );
}

void ForvoArticleRequest::requestFinished( QNetworkReply * r )
{
    Forvo::ForvoArticleRequest::requestFinished(r);
}

vector< sptr< Dictionary::Class > > makeDictionaries(
                                      Dictionary::Initializing &,
                                      Config::Forvo const & forvo,
                                      QNetworkAccessManager & mgr )
  THROW_SPEC( std::exception )
{
  vector< sptr< Dictionary::Class > > result;

  if ( forvo.enable )
  {
    QStringList codes = forvo.languageCodes.split( ',', QString::SkipEmptyParts );

    QSet< QString > usedCodes;

    for( int x = 0; x < codes.size(); ++x )
    {
      QString code = codes[ x ].simplified();

      if ( code.size() && !usedCodes.contains( code ) )
      {
        // Generate id

        QCryptographicHash hash( QCryptographicHash::Md5 );

        hash.addData( "Forvo source version 1.0" );
        hash.addData( code.toUtf8() );

        QString displayedCode( code.toLower() );

        if ( displayedCode.size() )
          displayedCode[ 0 ] = displayedCode[ 0 ].toUpper();

        result.push_back(
            new ForvoDictionary( hash.result().toHex().data(),
                                 QString( "Forvo (%1)" ).arg( displayedCode ).toUtf8().data(),
                                 forvo.apiKey, code, mgr ) );

        usedCodes.insert( code );
      }
    }
  }

  return result;
}

}
/* This file is (c) 2008-2012 Konstantin Isakov <ikm@goldendict.org>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE file */

#include "mediawiki.hh"
#include "wstring_qt.hh"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QtXml>
#include <list>
#include "gddebug.hh"
#include "audiolink.hh"
#include "langcoder.hh"
#include "qt4x5.hh"

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QRegularExpression>
#endif

namespace MediaWiki {

using namespace Dictionary;

namespace {

class MediaWikiDictionary: public Dictionary::Class
{
  string name;
  QString url, icon;
  QNetworkAccessManager & netMgr;
  quint32 langId;

public:

  MediaWikiDictionary( string const & id, string const & name_,
                       QString const & url_,
                       QString const & icon_,
                       QNetworkAccessManager & netMgr_ ):
    Dictionary::Class( id, vector< string >() ),
    name( name_ ),
    url( url_ ),
    icon( icon_ ),
    netMgr( netMgr_ ),
    langId( 0 )
  {
    int n = url.indexOf( "." );
    if( n == 2 || ( n > 3 && url[ n-3 ] == '/' ) )
      langId = LangCoder::code2toInt( url.mid( n - 2, 2 ).toLatin1().data() );
  }

  virtual string getName() throw()
  { return name; }

  virtual map< Property, string > getProperties() throw()
  { return map< Property, string >(); }

  virtual unsigned long getArticleCount() throw()
  { return 0; }

  virtual unsigned long getWordCount() throw()
  { return 0; }

  virtual sptr< WordSearchRequest > prefixMatch( wstring const &,
                                                 unsigned long maxResults ) THROW_SPEC( std::exception );

  virtual sptr< DataRequest > getArticle( wstring const &, vector< wstring > const & alts,
                                          wstring const &, bool )
    THROW_SPEC( std::exception );

  virtual quint32 getLangFrom() const
  { return langId; }

  virtual quint32 getLangTo() const
  { return langId; }

protected:

  virtual void loadIcon() throw();

};

void MediaWikiDictionary::loadIcon() throw()
{
  if ( dictionaryIconLoaded )
    return;

  if( !icon.isEmpty() )
  {
    QFileInfo fInfo(  QDir( Config::getConfigDir() ), icon );
    if( fInfo.isFile() )
      loadIconFromFile( fInfo.absoluteFilePath(), true );
  }
  if( dictionaryIcon.isNull() )
    dictionaryIcon = dictionaryNativeIcon = QIcon(":/icons/icon32_wiki.png");
  dictionaryIconLoaded = true;
}

class MediaWikiWordSearchRequest: public MediaWikiWordSearchRequestSlots
{
  sptr< QNetworkReply > netReply;
  bool livedLongEnough; // Indicates that the request has lived long enough
                        // to be destroyed prematurely. Used to prevent excessive
                        // network loads when typing search terms rapidly.
  bool isCancelling;

public:

  MediaWikiWordSearchRequest( wstring const &,
                              QString const & url, QNetworkAccessManager & mgr );

  ~MediaWikiWordSearchRequest();

  virtual void cancel();

protected:

  virtual void timerEvent( QTimerEvent * );

private:

  virtual void downloadFinished();
};

MediaWikiWordSearchRequest::MediaWikiWordSearchRequest( wstring const & str,
                                                        QString const & url,
                                                        QNetworkAccessManager & mgr ):
  livedLongEnough( false ), isCancelling( false )
{
  GD_DPRINTF( "request begin\n" );
  QUrl reqUrl( url + "/api.php?action=query&list=allpages&aplimit=40&format=xml" );

#if IS_QT_5
  Qt4x5::Url::addQueryItem( reqUrl, "apfrom", gd::toQString( str ).replace( '+', "%2B" ) );
#else
  reqUrl.addEncodedQueryItem( "apfrom", QUrl::toPercentEncoding( gd::toQString( str ) ) );
#endif

  netReply = mgr.get( QNetworkRequest( reqUrl ) );

  connect( netReply.get(), SIGNAL( finished() ),
           this, SLOT( downloadFinished() ) );

#ifndef QT_NO_OPENSSL

  connect( netReply.get(), SIGNAL( sslErrors( QList< QSslError > ) ),
           netReply.get(), SLOT( ignoreSslErrors() ) );

#endif

  // We start a timer to postpone early destruction, so a rapid type won't make
  // unnecessary network load
  startTimer( 200 );
}

void MediaWikiWordSearchRequest::timerEvent( QTimerEvent * ev )
{
  killTimer( ev->timerId() );
  livedLongEnough = true;

  if ( isCancelling )
    finish();
}

MediaWikiWordSearchRequest::~MediaWikiWordSearchRequest()
{
  GD_DPRINTF( "request end\n" );
}

void MediaWikiWordSearchRequest::cancel()
{
  // We either finish it in place, or in the timer handler
  isCancelling = true;

  if ( netReply.get() )
    netReply.reset();

  if ( livedLongEnough )
  {
    finish();
  }
  else
  {
    GD_DPRINTF("not long enough\n" );
  }
}

void MediaWikiWordSearchRequest::downloadFinished()
{
  if ( isCancelling || isFinished() ) // Was cancelled
    return;

  if ( netReply->error() == QNetworkReply::NoError )
  {
    QDomDocument dd;

    QString errorStr;
    int errorLine, errorColumn;
    bool finished = netReply->isFinished();
    QHash<QString, QString> requestHeaders;
    QList<QByteArray> requestHeaderList = netReply->request().rawHeaderList();
    for (auto header: requestHeaderList) {
        QString headerValue = netReply->request().rawHeader(header);
        requestHeaders[header] = headerValue;
    }

    QHash<QString, QString> headers;
    QList<QByteArray> headerList = netReply->rawHeaderList();
    for (auto header: headerList) {
        QString headerValue = netReply->rawHeader(header);
        headers[header] = headerValue;
    }
    QByteArray replyData = netReply->readAll();
    QString replyContent(replyData);

    if ( !dd.setContent( replyContent/*netReply.get()*/, false, &errorStr, &errorLine, &errorColumn  ) )
    {
      setErrorString( QString( tr( "XML parse error: %1 at %2,%3" ).
                               arg( errorStr ).arg( errorLine ).arg( errorColumn ) ) );
    }
    else
    {
      QDomNode pages = dd.namedItem( "api" ).namedItem( "query" ).namedItem( "allpages" );

      if ( !pages.isNull() )
      {
        QDomNodeList nl = pages.toElement().elementsByTagName( "p" );

        Mutex::Lock _( dataMutex );

        for( Qt4x5::Dom::size_type x = 0; x < nl.length(); ++x )
          matches.push_back( gd::toWString( nl.item( x ).toElement().attribute( "title" ) ) );
      }
    }
    GD_DPRINTF( "done.\n" );
  }
  else
    setErrorString( netReply->errorString() );

  finish();
}

class MediaWikiArticleRequest: public MediaWikiDataRequestSlots
{
  typedef std::list< std::pair< QNetworkReply *, bool > > NetReplies;
  NetReplies netReplies;
  QString url;

public:

  MediaWikiArticleRequest( wstring const & word, vector< wstring > const & alts,
                           QString const & url, QNetworkAccessManager & mgr,
                           Class * dictPtr_ );

  virtual void cancel();

private:

  void addQuery( QNetworkAccessManager & mgr, wstring const & word );

  virtual void requestFinished( QNetworkReply * );
  Class * dictPtr;
};

void MediaWikiArticleRequest::cancel()
{
  finish();
}

MediaWikiArticleRequest::MediaWikiArticleRequest( wstring const & str,
                                                  vector< wstring > const & alts,
                                                  QString const & url_,
                                                  QNetworkAccessManager & mgr,
                                                  Class * dictPtr_ ):
  url( url_ ), dictPtr( dictPtr_ )
{
  connect( &mgr, SIGNAL( finished( QNetworkReply * ) ),
           this, SLOT( requestFinished( QNetworkReply * ) ),
           Qt::QueuedConnection );
  
  addQuery(  mgr, str );

  for( unsigned x = 0; x < alts.size(); ++x )
    addQuery( mgr, alts[ x ] );
}

void MediaWikiArticleRequest::addQuery( QNetworkAccessManager & mgr,
                                        wstring const & str )
{
  gdDebug( "MediaWiki: requesting article %s\n", gd::toQString( str ).toUtf8().data() );

  QUrl reqUrl( url + "/api.php?action=parse&prop=text|revid&format=xml&redirects" );

#if IS_QT_5
  Qt4x5::Url::addQueryItem( reqUrl, "page", gd::toQString( str ).replace( '+', "%2B" ) );
#else
  reqUrl.addEncodedQueryItem( "page", QUrl::toPercentEncoding( gd::toQString( str ) ) );
#endif

  QNetworkReply * netReply = mgr.get( QNetworkRequest( reqUrl ) );
  
#ifndef QT_NO_OPENSSL

  connect( netReply, SIGNAL( sslErrors( QList< QSslError > ) ),
           netReply, SLOT( ignoreSslErrors() ) );

#endif

  netReplies.push_back( std::make_pair( netReply, false ) );
}

void MediaWikiArticleRequest::requestFinished( QNetworkReply * r )
{
  GD_DPRINTF( "Finished.\n" );

  if ( isFinished() ) // Was cancelled
    return;

  // Find this reply

  bool found = false;
  
  for( NetReplies::iterator i = netReplies.begin(); i != netReplies.end(); ++i )
  {
    if ( i->first == r )
    {
      i->second = true; // Mark as finished
      found = true;
      break;
    }
  }

  if ( !found )
  {
    // Well, that's not our reply, don't do anything
    return;
  }
  
  bool updated = false;

  for( ; netReplies.size() && netReplies.front().second; netReplies.pop_front() )
  {
    QNetworkReply * netReply = netReplies.front().first;
    
    if ( netReply->error() == QNetworkReply::NoError )
    {
      QDomDocument dd;
  
      QString errorStr;
      int errorLine, errorColumn;
  
      if ( !dd.setContent( netReply, false, &errorStr, &errorLine, &errorColumn  ) )
      {
        setErrorString( QString( tr( "XML parse error: %1 at %2,%3" ).
                                 arg( errorStr ).arg( errorLine ).arg( errorColumn ) ) );
      }
      else
      {
        QDomNode parseNode = dd.namedItem( "api" ).namedItem( "parse" );
  
        if ( !parseNode.isNull() && parseNode.toElement().attribute( "revid" ) != "0" )
        {
          QDomNode textNode = parseNode.namedItem( "text" );
  
          if ( !textNode.isNull() )
          {
            QString articleString = textNode.toElement().text();

            // Replace all ":" in links, remove '#' part in links to other articles
            int pos = 0;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            QRegularExpression regLinks( "<a\\s+href=\"/([^\"]+)\"" );
            QString articleNewString;
            QRegularExpressionMatchIterator it = regLinks.globalMatch( articleString );
            while( it.hasNext() )
            {
              QRegularExpressionMatch match = it.next();
              articleNewString += articleString.midRef( pos, match.capturedStart() - pos );
              pos = match.capturedEnd();

              QString link = match.captured( 1 );
#else
            QRegExp regLinks( "<a\\s+href=\"/([^\"]+)\"" );
            for( ; ; )
            {
              pos = regLinks.indexIn( articleString, pos );
              if( pos < 0 )
                break;
              QString link = regLinks.cap( 1 );
#endif
              if( link.indexOf( "://" ) >= 0 )
              {
                // External link
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
                articleNewString += match.captured();
#else
                pos += regLinks.cap().size();
#endif
                continue;
              }

              if( link.indexOf( ':' ) >= 0 )
                link.replace( ':', "%3A" );

              int n = link.indexOf( '#', 1 );
              if( n > 0 )
              {
                QString anchor = link.mid( n + 1 ).replace( '_', "%5F" );
                link.truncate( n );
                link += QString( "?gdanchor=%1" ).arg( anchor );
              }

              QString newLink = QString( "<a href=\"/%1\"" ).arg( link );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
              articleNewString += newLink;
            }
            if( pos )
            {
              articleNewString += articleString.midRef( pos );
              articleString = articleNewString;
              articleNewString.clear();
            }
#else
              articleString.replace( pos, regLinks.cap().size(), newLink );
              pos += newLink.size();
            }
#endif

            QUrl wikiUrl( url );
            wikiUrl.setPath( "/" );
  
            // Update any special index.php pages to be absolute
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            articleString.replace( QRegularExpression( "<a\\shref=\"(/([\\w]*/)*index.php\\?)" ),
                                   QString( "<a href=\"%1\\1" ).arg( wikiUrl.toString() ) );
#else
            articleString.replace( QRegExp( "<a\\shref=\"(/(\\w*/)*index.php\\?)" ),
                                   QString( "<a href=\"%1\\1" ).arg( wikiUrl.toString() ) );
#endif

            // audio tag
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            QRegularExpression reg1( "<audio\\s.+?</audio>",
                                     QRegularExpression::CaseInsensitiveOption
                                     | QRegularExpression::DotMatchesEverythingOption );
            QRegularExpression reg2( "<source\\s+src=\"([^\"]+)",
                                     QRegularExpression::CaseInsensitiveOption );
            pos = 0;
            it = reg1.globalMatch( articleString );
            while( it.hasNext() )
            {
              QRegularExpressionMatch match = it.next();
              articleNewString += articleString.midRef( pos, match.capturedStart() - pos );
              pos = match.capturedEnd();

              QString tag = match.captured();
              QRegularExpressionMatch match2 = reg2.match( tag );
              if( match2.hasMatch() )
              {
                QString ref = match2.captured( 1 );
                QString audio_url = "<a href=\"" + ref
                                    + "\"><img src=\"qrcx://localhost/icons/playsound.png\" border=\"0\" align=\"absmiddle\" alt=\"Play\"/></a>";
                articleNewString += audio_url;
              }
              else
                articleNewString += match.captured();
            }
            if( pos )
            {
              articleNewString += articleString.midRef( pos );
              articleString = articleNewString;
              articleNewString.clear();
            }
#else
            QRegExp reg1( "<audio\\s.+</audio>", Qt::CaseInsensitive, QRegExp::RegExp2 );
            reg1.setMinimal( true );
            QRegExp reg2( "<source\\s+src=\"([^\"]+)", Qt::CaseInsensitive );
            pos = 0;
            for( ; ; )
            {
              pos = reg1.indexIn( articleString, pos );
              if( pos >= 0 )
              {
                QString tag = reg1.cap();
                if( reg2.indexIn( tag ) >= 0 )
                {
                  QString ref = reg2.cap( 1 );
                  QString audio_url = "<a href=\"" + ref
                                      + "\"><img src=\"qrcx://localhost/icons/playsound.png\" border=\"0\" align=\"absmiddle\" alt=\"Play\"/></a>";
                  articleString.replace( pos, tag.length(), audio_url );
                }
                pos += 1;
              }
              else
                break;
            }
#endif
            // audio url
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            articleString.replace( QRegularExpression( "<a\\s+href=\"(//upload\\.wikimedia\\.org/wikipedia/[^\"'&]*\\.ogg(?:\\.mp3|))\"" ),
#else
            articleString.replace( QRegExp( "<a\\s+href=\"(//upload\\.wikimedia\\.org/wikipedia/[^\"'&]*\\.ogg(?:\\.mp3|))\"" ),
#endif
                                   QString::fromStdString( addAudioLink( string( "\"" ) + wikiUrl.scheme().toStdString() + ":\\1\"",
                                                                         this->dictPtr->getId() ) + "<a href=\"" + wikiUrl.scheme().toStdString() + ":\\1\"" ) );

            // Add url scheme to image source urls
            articleString.replace( " src=\"//", " src=\"" + wikiUrl.scheme() + "://" );
            //fix src="/foo/bar/Baz.png"
            articleString.replace( "src=\"/", "src=\"" + wikiUrl.toString() );

            // Remove the /wiki/ prefix from links
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            articleString.replace( QRegularExpression( "<a\\s+href=\"/wiki/" ), "<a href=\"" );
#else
            articleString.replace( QRegExp( "<a\\s+href=\"/wiki/" ), "<a href=\"" );
#endif

            //fix audio
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            articleString.replace( QRegularExpression( "<button\\s+[^>]*(upload\\.wikimedia\\.org/wikipedia/commons/[^\"'&]*\\.ogg)[^>]*>\\s*<[^<]*</button>" ),
#else
            articleString.replace( QRegExp( "<button\\s+[^>]*(upload\\.wikimedia\\.org/wikipedia/commons/[^\"'&]*\\.ogg)[^>]*>\\s*<[^<]*</button>"),
#endif
                                            QString::fromStdString(addAudioLink( string( "\"" ) + wikiUrl.scheme().toStdString() + "://\\1\"", this->dictPtr->getId() ) +
                                            "<a href=\"" + wikiUrl.scheme().toStdString() + "://\\1\"><img src=\"qrcx://localhost/icons/playsound.png\" border=\"0\" alt=\"Play\"></a>" ) );
            // In those strings, change any underscores to spaces
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            pos = 0;
            QRegularExpression rxLink( "<a\\s+href=\"[^/:\">#]+" );
            it = rxLink.globalMatch( articleString );
            while( it.hasNext() )
            {
              QRegularExpressionMatch match = it.next();
              for( int i = match.capturedStart() + 9; i < match.capturedEnd(); i++ )
                if( articleString.at( i ) == QChar( '_') )
                  articleString[ i ] = ' ';
            }
#else
            for( ; ; )
            {
              QString before = articleString;
              articleString.replace( QRegExp( "<a href=\"([^/:\">#]*)_" ), "<a href=\"\\1 " );
  
              if ( articleString == before )
                break;
            }
#endif
            //fix file: url
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            articleString.replace( QRegularExpression( "<a\\s+href=\"([^:/\"]*file%3A[^/\"]+\")",
                                                       QRegularExpression::CaseInsensitiveOption ),
#else
            articleString.replace( QRegExp("<a\\s+href=\"([^:/\"]*file%3A[^/\"]+\")", Qt::CaseInsensitive ),
#endif
                                   QString( "<a href=\"%1/index.php?title=\\1" ).arg( url ));

            // Add url scheme to other urls like  "//xxx"
            articleString.replace( " href=\"//", " href=\"" + wikiUrl.scheme() + "://" );

            // Fix urls in "srcset" attribute
            pos = 0;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            QRegularExpression regSrcset( " srcset\\s*=\\s*\"/[^\"]+\"" );
            it = regSrcset.globalMatch( articleString );
            while( it.hasNext() )
            {
              QRegularExpressionMatch match = it.next();
              articleNewString += articleString.midRef( pos, match.capturedStart() - pos );
              pos = match.capturedEnd();

              QString srcset = match.captured();
#else
            QRegExp regSrcset( " srcset\\s*=\\s*\"/([^\"]+)\"" );
            for( ; ; )
            {
              pos = regSrcset.indexIn( articleString, pos );
              if( pos < 0 )
                break;
              QString srcset = regSrcset.cap();
#endif
              QString newSrcset = srcset.replace( "//", wikiUrl.scheme() + "://" );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
              articleNewString += newSrcset;
            }
            if( pos )
            {
              articleNewString += articleString.midRef( pos );
              articleString = articleNewString;
              articleNewString.clear();
            }
#else
              articleString.replace( pos, regSrcset.cap().size(), newSrcset );
              pos += newSrcset.size();
            }
#endif

            QByteArray articleBody = articleString.toUtf8();

            articleBody.prepend( dictPtr->isToLanguageRTL() ? "<div class=\"mwiki\" dir=\"rtl\">" :
                                                              "<div class=\"mwiki\">" );
            articleBody.append( "</div>" );
  
            Mutex::Lock _( dataMutex );

            size_t prevSize = data.size();
            
            data.resize( prevSize + articleBody.size() );
  
            memcpy( &data.front() + prevSize, articleBody.data(), articleBody.size() );
  
            hasAnyData = true;

            updated = true;
          }
        }
      }
      GD_DPRINTF( "done.\n" );
    }
    else
      setErrorString( netReply->errorString() );

    disconnect( netReply, 0, 0, 0 );
    netReply->deleteLater();
  }

  if ( netReplies.empty() )
    finish();
  else
  if ( updated )
    update();
}

sptr< WordSearchRequest > MediaWikiDictionary::prefixMatch( wstring const & word,
                                                            unsigned long maxResults )
  THROW_SPEC( std::exception )
{
  (void) maxResults;
  if ( word.size() > 80 )
  {
    // Don't make excessively large queries -- they're fruitless anyway

    return new WordSearchRequestInstant();
  }
  else
    return new MediaWikiWordSearchRequest( word, url, netMgr );
}

sptr< DataRequest > MediaWikiDictionary::getArticle( wstring const & word,
                                                     vector< wstring > const & alts,
                                                     wstring const &, bool )
  THROW_SPEC( std::exception )
{
  if ( word.size() > 80 )
  {
    // Don't make excessively large queries -- they're fruitless anyway

    return new DataRequestInstant( false );
  }
  else
    return new MediaWikiArticleRequest( word, alts, url, netMgr, this );
}

}

vector< sptr< Dictionary::Class > > makeDictionaries(
                                      Dictionary::Initializing &,
                                      Config::MediaWikis const & wikis,
                                      QNetworkAccessManager & mgr )
  THROW_SPEC( std::exception )
{
  vector< sptr< Dictionary::Class > > result;

  for( int x = 0; x < wikis.size(); ++x )
  {
    if ( wikis[ x ].enabled )
      result.push_back( new MediaWikiDictionary( wikis[ x ].id.toStdString(),
                                                 wikis[ x ].name.toUtf8().data(),
                                                 wikis[ x ].url,
                                                 wikis[ x ].icon,
                                                 mgr ) );
  }

  return result;
}

}
/* This file is (c) 2008-2012 Konstantin Isakov <ikm@goldendict.org>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE file */

#include "programs.hh"
#include "audiolink.hh"
#include "htmlescape.hh"
#include "utf8.hh"
#include "wstring_qt.hh"
#include "parsecmdline.hh"
#include "iconv.hh"
#include "qt4x5.hh"

#include <QDir>
#include <QFileInfo>

namespace Programs {

using namespace Dictionary;

namespace {

class ProgramsDictionary: public Dictionary::Class
{
  Config::Program prg;
public:

  ProgramsDictionary( Config::Program const & prg_ ):
    Dictionary::Class( prg_.id.toStdString(), vector< string >() ),
    prg( prg_ )
  {
  }

  virtual string getName() throw()
  { return prg.name.toUtf8().data(); }

  virtual map< Property, string > getProperties() throw()
  { return map< Property, string >(); }

  virtual unsigned long getArticleCount() throw()
  { return 0; }

  virtual unsigned long getWordCount() throw()
  { return 0; }

  virtual sptr< WordSearchRequest > prefixMatch( wstring const & word,
                                                 unsigned long maxResults )
    THROW_SPEC( std::exception );

  virtual sptr< DataRequest > getArticle( wstring const &,
                                          vector< wstring > const & alts,
                                          wstring const &, bool )
    THROW_SPEC( std::exception );

protected:

  virtual void loadIcon() throw();
};

sptr< WordSearchRequest > ProgramsDictionary::prefixMatch( wstring const & word,
                                                           unsigned long /*maxResults*/ )
  THROW_SPEC( std::exception )
{
  if ( prg.type == Config::Program::PrefixMatch )
    return new ProgramWordSearchRequest( gd::toQString( word ), prg );
  else
  {
    sptr< WordSearchRequestInstant > sr = new WordSearchRequestInstant;

    sr->setUncertain( true );

    return sr;
  }
}

sptr< Dictionary::DataRequest > ProgramsDictionary::getArticle(
  wstring const & word, vector< wstring > const &, wstring const &, bool )
  THROW_SPEC( std::exception )
{
  switch( prg.type )
  {
    case Config::Program::Audio:
    {
      // Audio results are instantaneous
      string result;

      string wordUtf8( Utf8::encode( word ) );

      result += "<table class=\"programs_play\"><tr>";

      QUrl url;
      url.setScheme( "gdprg" );
      url.setHost( QString::fromUtf8( getId().c_str() ) );
      url.setPath( Qt4x5::Url::ensureLeadingSlash( QString::fromUtf8( wordUtf8.c_str() ) ) );

      string ref = string( "\"" ) + url.toEncoded().data() + "\"";

      result += addAudioLink( ref, getId() );

      result += "<td><a href=" + ref + "><img src=\"qrcx://localhost/icons/playsound.png\" border=\"0\" alt=\"Play\"/></a></td>";
      result += "<td><a href=" + ref + ">" +
                Html::escape( wordUtf8 ) + "</a></td>";
      result += "</tr></table>";

      sptr< DataRequestInstant > ret = new DataRequestInstant( true );

      ret->getData().resize( result.size() );

      memcpy( &(ret->getData().front()), result.data(), result.size() );
      return ret;
    }

    case Config::Program::Html:
    case Config::Program::PlainText:
      return new ProgramDataRequest( gd::toQString( word ), prg );

    default:
      return new DataRequestInstant( false );
  }
}

void ProgramsDictionary::loadIcon() throw()
{
  if ( dictionaryIconLoaded )
    return;

  if( !prg.iconFilename.isEmpty() )
  {
    QFileInfo fInfo(  QDir( Config::getConfigDir() ), prg.iconFilename );
    if( fInfo.isFile() )
      loadIconFromFile( fInfo.absoluteFilePath(), true );
  }
  if( dictionaryIcon.isNull() )
    dictionaryIcon = dictionaryNativeIcon = QIcon(":/icons/programs.png");
  dictionaryIconLoaded = true;
}

}

RunInstance::RunInstance(): process( this )
{
  connect( this, SIGNAL(processFinished()), this,
           SLOT(handleProcessFinished()), Qt::QueuedConnection );
  connect( &process, SIGNAL(finished(int)), this, SIGNAL(processFinished()));
  connect( &process, SIGNAL(error(QProcess::ProcessError)), this,
           SIGNAL(processFinished()) );
}

bool RunInstance::start( Config::Program const & prg, QString const & word,
                         QString & error )
{
  QStringList args = parseCommandLine( prg.commandLine );

  if ( !args.empty() )
  {
    QString programName = args.first();
    args.pop_front();

    bool writeToStdInput = true;

    for( int x = 0; x < args.size(); ++x )
      if( args[ x ].indexOf( "%GDWORD%" ) >= 0 )
      {
        writeToStdInput = false;
        args[ x ].replace( "%GDWORD%", word );
      }

    process.start( programName, args );
    if( writeToStdInput )
    {
      process.write( word.toLocal8Bit() );
      process.closeWriteChannel();
    }

    return true;
  }
  else
  {
    error = tr( "No program name was given." );
    return false;
  }
}

void RunInstance::handleProcessFinished()
{
  // It seems that sometimes the process isn't finished yet despite being
  // signalled as such. So we wait for it here, which should hopefully be
  // nearly instant.
  process.waitForFinished();

  QByteArray output = process.readAllStandardOutput();

  QString error;
  if ( process.exitStatus() != QProcess::NormalExit )
    error = tr( "The program has crashed." );
  else
  if ( int code = process.exitCode() )
    error = tr( "The program has returned exit code %1." ).arg( code );

  if ( !error.isEmpty() )
  {
    QByteArray err = process.readAllStandardError();

    if ( !err.isEmpty() )
      error += "\n\n" + QString::fromLocal8Bit( err );
  }

  emit finished( output, error );
}

ProgramDataRequest::ProgramDataRequest( QString const & word,
                                        Config::Program const & prg_ ):
  prg( prg_ )
{
  connect( &instance, SIGNAL(finished(QByteArray,QString)),
           this, SLOT(instanceFinished(QByteArray,QString)) );

  QString error;
  if ( !instance.start( prg, word, error ) )
  {
    setErrorString( error );
    finish();
  }
}

void ProgramDataRequest::instanceFinished( QByteArray output, QString error )
{
  QString prog_output;
  if ( !isFinished() )
  {
    if ( !output.isEmpty() )
    {
      string result = "<div class='programs_";

      switch( prg.type )
      {
      case Config::Program::PlainText:
        result += "plaintext'>";
        try
        {
          // Check BOM if present
          unsigned char * uchars = reinterpret_cast< unsigned char * >( output.data() );
          if( output.length() >= 2 && uchars[ 0 ] == 0xFF && uchars[ 1 ] == 0xFE )
          {
            int size = output.length() - 2;
            if( size & 1 )
              size -= 1;
            string res= Iconv::toUtf8( "UTF-16LE", output.data() + 2, size );
            prog_output = QString::fromUtf8( res.c_str(), res.size() );
          }
          else
          if( output.length() >= 2 && uchars[ 0 ] == 0xFE && uchars[ 1 ] == 0xFF )
          {
            int size = output.length() - 2;
            if( size & 1 )
              size -= 1;
            string res = Iconv::toUtf8( "UTF-16BE", output.data() + 2, size );
            prog_output = QString::fromUtf8( res.c_str(), res.size() );
          }
          else
          if( output.length() >= 3 && uchars[ 0 ] == 0xEF && uchars[ 1 ] == 0xBB && uchars[ 2 ] == 0xBF )
          {
            prog_output = QString::fromUtf8( output.data() + 3, output.length() - 3 );
          }
          else
          {
            // No BOM, assume local 8-bit encoding
            prog_output = QString::fromLocal8Bit( output );
          }
        }
        catch( std::exception & e )
        {
          error = e.what();
        }
        result += Html::preformat( prog_output.toUtf8().data() );
        break;
      default:
        result += "html'>";
        try
        {
          // Check BOM if present
          unsigned char * uchars = reinterpret_cast< unsigned char * >( output.data() );
          if( output.length() >= 2 && uchars[ 0 ] == 0xFF && uchars[ 1 ] == 0xFE )
          {
            int size = output.length() - 2;
            if( size & 1 )
              size -= 1;
            result += Iconv::toUtf8( "UTF-16LE", output.data() + 2, size );
          }
          else
          if( output.length() >= 2 && uchars[ 0 ] == 0xFE && uchars[ 1 ] == 0xFF )
          {
            int size = output.length() - 2;
            if( size & 1 )
              size -= 1;
            result += Iconv::toUtf8( "UTF-16BE", output.data() + 2, size );
          }
          else
          if( output.length() >= 3 && uchars[ 0 ] == 0xEF && uchars[ 1 ] == 0xBB && uchars[ 2 ] == 0xBF )
          {
            result += output.data() + 3;
          }
          else
          {
            // We assume html data is in utf8 encoding already.
            result += output.data();
          }
        }
        catch( std::exception & e )
        {
          error = e.what();
        }
      }

      result += "</div>";

      Mutex::Lock _( dataMutex );
      data.resize( result.size() );
      memcpy( data.data(), result.data(), data.size() );
      hasAnyData = true;
    }

    if ( !error.isEmpty() )
      setErrorString( error );

    finish();
  }
}

void ProgramDataRequest::cancel()
{
  finish();
}

ProgramWordSearchRequest::ProgramWordSearchRequest( QString const & word,
                                                    Config::Program const & prg_ ):
  prg( prg_ )
{
  connect( &instance, SIGNAL(finished(QByteArray,QString)),
           this, SLOT(instanceFinished(QByteArray,QString)) );

  QString error;
  if ( !instance.start( prg, word, error ) )
  {
    setErrorString( error );
    finish();
  }
}

void ProgramWordSearchRequest::instanceFinished( QByteArray output, QString error )
{
  if ( !isFinished() )
  {
    // Handle any Windows artifacts
    output.replace( "\r\n", "\n" );
    QStringList result =
      QString::fromUtf8( output ).split( "\n", QString::SkipEmptyParts );

    for( int x = 0; x < result.size(); ++x )
      matches.push_back( Dictionary::WordMatch( gd::toWString( result[ x ] ) ) );

    if ( !error.isEmpty() )
      setErrorString( error );

    finish();
  }
}

void ProgramWordSearchRequest::cancel()
{
  finish();
}

vector< sptr< Dictionary::Class > > makeDictionaries(
  Config::Programs const & programs )
  THROW_SPEC( std::exception )
{
  vector< sptr< Dictionary::Class > > result;

  for( Config::Programs::const_iterator i = programs.begin();
       i != programs.end(); ++i )
    if ( i->enabled )
      result.push_back( new ProgramsDictionary( *i ) );

  return result;
}

}
/* This file is (c) 2008-2012 Konstantin Isakov <ikm@goldendict.org>
 * Part of GoldenDict. Licensed under GPLv3 or later, see the LICENSE file */

#include "website.hh"
#include "wstring_qt.hh"
#include "utf8.hh"
#include <QUrl>
#include <QTextCodec>
#include <QDir>
#include <QFileInfo>
#include "gddebug.hh"

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

namespace WebSite {

using namespace Dictionary;

namespace {

class WebSiteDictionary: public Dictionary::Class
{
  string name;
  QByteArray urlTemplate;
  QString iconFilename;
  bool inside_iframe;
  QNetworkAccessManager & netMgr;

public:

  WebSiteDictionary( string const & id, string const & name_,
                     QString const & urlTemplate_,
                     QString const & iconFilename_,
                     bool inside_iframe_,
                     QNetworkAccessManager & netMgr_ ):
    Dictionary::Class( id, vector< string >() ),
    name( name_ ),
    urlTemplate( QUrl( urlTemplate_ ).toEncoded() ),
    iconFilename( iconFilename_ ),
    inside_iframe( inside_iframe_ ),
    netMgr( netMgr_ )
  {
    dictionaryDescription = urlTemplate_;
  }

  virtual string getName() throw()
  { return name; }

  virtual map< Property, string > getProperties() throw()
  { return map< Property, string >(); }

  virtual unsigned long getArticleCount() throw()
  { return 0; }

  virtual unsigned long getWordCount() throw()
  { return 0; }

  virtual sptr< WordSearchRequest > prefixMatch( wstring const & word,
                                                 unsigned long ) THROW_SPEC( std::exception );

  virtual sptr< DataRequest > getArticle( wstring const &,
                                          vector< wstring > const & alts,
                                          wstring const & context, bool )
    THROW_SPEC( std::exception );

  virtual sptr< Dictionary::DataRequest > getResource( string const & name ) THROW_SPEC( std::exception );

  void isolateWebCSS( QString & css );

protected:

  virtual void loadIcon() throw();
};

sptr< WordSearchRequest > WebSiteDictionary::prefixMatch( wstring const & /*word*/,
                                                          unsigned long ) THROW_SPEC( std::exception )
{
  sptr< WordSearchRequestInstant > sr = new WordSearchRequestInstant;

  sr->setUncertain( true );

  return sr;
}

void WebSiteDictionary::isolateWebCSS( QString & css )
{
  isolateCSS( css, ".website" );
}

class WebSiteArticleRequest: public WebSiteDataRequestSlots
{
  QNetworkReply * netReply;
  QString url;
  Class * dictPtr;
  QNetworkAccessManager & mgr;

public:

  WebSiteArticleRequest( QString const & url, QNetworkAccessManager & _mgr,
                         Class * dictPtr_ );
  ~WebSiteArticleRequest()
  {}

  virtual void cancel();

private:

  virtual void requestFinished( QNetworkReply * );
  static QTextCodec * codecForHtml( QByteArray const & ba );
};

void WebSiteArticleRequest::cancel()
{
  finish();
}

WebSiteArticleRequest::WebSiteArticleRequest( QString const & url_,
                                              QNetworkAccessManager & _mgr,
                                              Class * dictPtr_ ):
  url( url_ ), dictPtr( dictPtr_ ), mgr( _mgr )
{
  connect( &mgr, SIGNAL( finished( QNetworkReply * ) ),
           this, SLOT( requestFinished( QNetworkReply * ) ),
           Qt::QueuedConnection );

  QUrl reqUrl( url );

  netReply = mgr.get( QNetworkRequest( reqUrl ) );

#ifndef QT_NO_OPENSSL
  connect( netReply, SIGNAL( sslErrors( QList< QSslError > ) ),
           netReply, SLOT( ignoreSslErrors() ) );
#endif
}

QTextCodec * WebSiteArticleRequest::codecForHtml( QByteArray const & ba )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
  return QTextCodec::codecForHtml( ba, 0 );
#else
// Implementation taken from Qt 5 sources
// Function from Qt 4 can't recognize charset name inside single quotes

  QByteArray header = ba.left( 1024 ).toLower();
  int pos = header.indexOf( "meta " );
  if (pos != -1) {
    pos = header.indexOf( "charset=", pos );
    if (pos != -1) {
      pos += qstrlen( "charset=" );

      int pos2 = pos;
      while ( ++pos2 < header.size() )
      {
        char ch = header.at( pos2 );
        if( ch != '\"' && ch != '\'' && ch != ' ' )
          break;
      }

      // The attribute can be closed with either """, "'", ">" or "/",
      // none of which are valid charset characters.

      while ( pos2++ < header.size() )
      {
        char ch = header.at( pos2 );
        if( ch == '\"' || ch == '\'' || ch == '>' || ch == '/' )
        {
          QByteArray name = header.mid( pos, pos2 - pos );
          if ( name == "unicode" )
            name = QByteArray( "UTF-8" );

          return QTextCodec::codecForName(name);
        }
      }
    }
  }
  return 0;
#endif
}

void WebSiteArticleRequest::requestFinished( QNetworkReply * r )
{
  if ( isFinished() ) // Was cancelled
    return;

  if ( r != netReply )
  {
    // Well, that's not our reply, don't do anything
    return;
  }

  if ( netReply->error() == QNetworkReply::NoError )
  {
    // Check for redirect reply

    QVariant possibleRedirectUrl = netReply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    QUrl redirectUrl = possibleRedirectUrl.toUrl();
    if( !redirectUrl.isEmpty() )
    {
      disconnect( netReply, 0, 0, 0 );
      netReply->deleteLater();
      netReply = mgr.get( QNetworkRequest( redirectUrl ) );
#ifndef QT_NO_OPENSSL
      connect( netReply, SIGNAL( sslErrors( QList< QSslError > ) ),
               netReply, SLOT( ignoreSslErrors() ) );
#endif
      return;
    }

    // Handle reply data

    QByteArray replyData = netReply->readAll();
    QString articleString;

    QTextCodec * codec = WebSiteArticleRequest::codecForHtml( replyData );
    if( codec )
      articleString = codec->toUnicode( replyData );
    else
      articleString = QString::fromUtf8( replyData );

    // Change links from relative to absolute

    QString root = netReply->url().scheme() + "://" + netReply->url().host();
    QString base = root + netReply->url().path();
    while( !base.isEmpty() && !base.endsWith( "/" ) )
      base.chop( 1 );

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    QRegularExpression tags( "<\\s*(a|link|img|script)\\s+[^>]*(src|href)\\s*=\\s*['\"][^>]+>",
                             QRegularExpression::CaseInsensitiveOption );
    QRegularExpression links( "\\b(src|href)\\s*=\\s*(['\"])([^'\"]+['\"])",
                              QRegularExpression::CaseInsensitiveOption );
    int pos = 0;
    QString articleNewString;
    QRegularExpressionMatchIterator it = tags.globalMatch( articleString );
    while( it.hasNext() )
    {
      QRegularExpressionMatch match = it.next();
      articleNewString += articleString.midRef( pos, match.capturedStart() - pos );
      pos = match.capturedEnd();

      QString tag = match.captured();

      QRegularExpressionMatch match_links = links.match( tag );
      if( !match_links.hasMatch() )
      {
        articleNewString += tag;
        continue;
      }

      QString url = match_links.captured( 3 );

      if( url.indexOf( ":/" ) >= 0 || url.indexOf( "data:" ) >= 0
          || url.indexOf( "mailto:" ) >= 0 || url.startsWith( "#" )
          || url.startsWith( "javascript:" ) )
      {
        // External link, anchor or base64-encoded data
        articleNewString += tag;
        continue;
      }

      QString newUrl = match_links.captured( 1 ) + "=" + match_links.captured( 2 );
      if( url.startsWith( "//" ) )
        newUrl += netReply->url().scheme() + ":";
      else
      if( url.startsWith( "/" ) )
        newUrl += root;
      else
        newUrl += base;
      newUrl += match_links.captured( 3 );

      tag.replace( match_links.capturedStart(), match_links.capturedLength(), newUrl );
      articleNewString += tag;
    }
    if( pos )
    {
      articleNewString += articleString.midRef( pos );
      articleString = articleNewString;
      articleNewString.clear();
    }

    // Redirect CSS links to own handler

    QString prefix = QString( "bres://" ) + dictPtr->getId().c_str() + "/";
    QRegularExpression linkTags( "(<\\s*link\\s[^>]*rel\\s*=\\s*['\"]stylesheet['\"]\\s+[^>]*href\\s*=\\s*['\"])([^'\"]+)://([^'\"]+['\"][^>]+>)",
                                 QRegularExpression::CaseInsensitiveOption );
    pos = 0;
    it = linkTags.globalMatch( articleString );
    while( it.hasNext() )
    {
      QRegularExpressionMatch match = it.next();
      articleNewString += articleString.midRef( pos, match.capturedStart() - pos );
      pos = match.capturedEnd();

      QString newTag = match.captured( 1 ) + prefix + match.captured( 2 )
                       + "/" + match.captured( 3 );
      articleNewString += newTag;
    }
    if( pos )
    {
      articleNewString += articleString.midRef( pos );
      articleString = articleNewString;
      articleNewString.clear();
    }
#else
    QRegExp tags( "<\\s*(a|link|img|script)\\s+[^>]*(src|href)\\s*=\\s*['\"][^>]+>",
                  Qt::CaseInsensitive, QRegExp::RegExp2 );
    QRegExp links( "\\b(src|href)\\s*=\\s*(['\"])([^'\"]+['\"])",
                   Qt::CaseInsensitive, QRegExp::RegExp2 );
    int pos = 0;
    while( pos >= 0 )
    {
      pos = articleString.indexOf( tags, pos );
      if( pos < 0 )
        break;

      QString tag = tags.cap();

      int linkPos = tag.indexOf( links );
      if( linkPos < 0 )
      {
        pos += tag.size();
        continue;
      }

      QString url = links.cap( 3 );

      if( url.indexOf( ":/" ) >= 0 || url.indexOf( "data:" ) >= 0
          || url.indexOf( "mailto:" ) >= 0 || url.startsWith( "#" )
          || url.startsWith( "javascript:" ) )
      {
        // External link, anchor or base64-encoded data
        pos += tag.size();
        continue;
      }

      QString newUrl = links.cap( 1 ) + "=" + links.cap( 2 );
      if( url.startsWith( "//" ) )
        newUrl += netReply->url().scheme() + ":";
      else
      if( url.startsWith( "/" ) )
        newUrl += root;
      else
        newUrl += base;
      newUrl += links.cap( 3 );

      tag.replace( linkPos, links.cap().size(), newUrl );
      articleString.replace( pos, tags.cap().size(), tag );

      pos += tag.size();
    }

    // Redirect CSS links to own handler

    QString prefix = QString( "bres://" ) + dictPtr->getId().c_str() + "/";
    QRegExp linkTags( "(<\\s*link\\s[^>]*rel\\s*=\\s*['\"]stylesheet['\"]\\s+[^>]*href\\s*=\\s*['\"])([^'\"]+)://([^'\"]+['\"][^>]+>)",
                  Qt::CaseInsensitive, QRegExp::RegExp2 );
    pos = 0;
    while( pos >= 0 )
    {
      pos = articleString.indexOf( linkTags, pos );
      if( pos < 0 )
        break;

      QString newTag = linkTags.cap( 1 ) + prefix + linkTags.cap( 2 )
                       + "/" + linkTags.cap( 3 );
      articleString.replace( pos, linkTags.cap().size(), newTag );
      pos += newTag.size();
    }
#endif
    // Check for unclosed <span> and <div>

    int openTags = articleString.count( QRegExp( "<\\s*span\\b", Qt::CaseInsensitive ) );
    int closedTags = articleString.count( QRegExp( "<\\s*/span\\s*>", Qt::CaseInsensitive ) );
    while( openTags > closedTags )
    {
      articleString += "</span>";
      closedTags += 1;
    }

    openTags = articleString.count( QRegExp( "<\\s*div\\b", Qt::CaseInsensitive ) );
    closedTags = articleString.count( QRegExp( "<\\s*/div\\s*>", Qt::CaseInsensitive ) );
    while( openTags > closedTags )
    {
      articleString += "</div>";
      closedTags += 1;
    }

    // See Issue #271: A mechanism to clean-up invalid HTML cards.
    articleString += "</font>""</font>""</font>""</font>""</font>""</font>"
                     "</font>""</font>""</font>""</font>""</font>""</font>"
                     "</b></b></b></b></b></b></b></b>"
                     "</i></i></i></i></i></i></i></i>"
                     "</a></a></a></a></a></a></a></a>";

    QByteArray articleBody = articleString.toUtf8();

    QString divStr = QString( "<div class=\"website\"" );
    divStr += dictPtr->isToLanguageRTL() ? " dir=\"rtl\">" : ">";

    articleBody.prepend( divStr.toUtf8() );
    articleBody.append( "</div>" );

    articleBody.prepend( "<div class=\"website_padding\"></div>" );

    Mutex::Lock _( dataMutex );

    size_t prevSize = data.size();

    data.resize( prevSize + articleBody.size() );

    memcpy( &data.front() + prevSize, articleBody.data(), articleBody.size() );

    hasAnyData = true;

  }
  else
  {
    if( netReply->url().scheme() == "file" )
    {
      gdWarning( "WebSites: Failed loading article from \"%s\", reason: %s\n", dictPtr->getName().c_str(),
                 netReply->errorString().toUtf8().data() );
    }
    else
    {
      setErrorString( netReply->errorString() );
    }
  }

  disconnect( netReply, 0, 0, 0 );
  netReply->deleteLater();

  finish();
}

sptr< DataRequest > WebSiteDictionary::getArticle( wstring const & str,
                                                   vector< wstring > const &,
                                                   wstring const & context, bool )
  THROW_SPEC( std::exception )
{
  QByteArray urlString;

  // Context contains the right url to go to
  if ( context.size() )
    urlString = Utf8::encode( context ).c_str();
  else
  {
    urlString = urlTemplate;

    QString inputWord = gd::toQString( str );

    urlString.replace( "%25GDWORD%25", inputWord.toUtf8().toPercentEncoding() );

    QTextCodec *codec = QTextCodec::codecForName( "Windows-1251" );
    if( codec )
      urlString.replace( "%25GD1251%25", codec->fromUnicode( inputWord ).toPercentEncoding() );

    codec = QTextCodec::codecForName( "Big-5" );
    if( codec )
      urlString.replace( "%25GDBIG5%25", codec->fromUnicode( inputWord ).toPercentEncoding() );

    codec = QTextCodec::codecForName( "Big5-HKSCS" );
    if( codec )
      urlString.replace( "%25GDBIG5HKSCS%25", codec->fromUnicode( inputWord ).toPercentEncoding() );

    codec = QTextCodec::codecForName( "Shift-JIS" );
    if( codec )
      urlString.replace( "%25GDSHIFTJIS%25", codec->fromUnicode( inputWord ).toPercentEncoding() );

    codec = QTextCodec::codecForName( "GB18030" );
    if( codec )
      urlString.replace( "%25GDGBK%25", codec->fromUnicode( inputWord ).toPercentEncoding() );


    // Handle all ISO-8859 encodings
    for( int x = 1; x <= 16; ++x )
    {
      codec = QTextCodec::codecForName( QString( "ISO 8859-%1" ).arg( x ).toLatin1() );
      if( codec )
        urlString.replace( QString( "%25GDISO%1%25" ).arg( x ), codec->fromUnicode( inputWord ).toPercentEncoding() );

      if ( x == 10 )
        x = 12; // Skip encodings 11..12, they don't exist
    }
  }

  if( inside_iframe )
  {
    // Just insert link in <iframe> tag

    sptr< DataRequestInstant > dr = new DataRequestInstant( true );

    string result = "<div class=\"website_padding\"></div>";

    result += string( "<iframe id=\"gdexpandframe-" ) + getId() +
                      "\" src=\"" + urlString.data() +
                      "\" onmouseover=\"processIframeMouseOver('gdexpandframe-" + getId() + "');\" "
                      "onmouseout=\"processIframeMouseOut();\" "
                      "scrolling=\"no\" marginwidth=\"0\" marginheight=\"0\" "
                      "frameborder=\"0\" vspace=\"0\" hspace=\"0\" "
                      "style=\"overflow:visible; width:100%; display:none;\">"
                      "</iframe>";

    dr->getData().resize( result.size() );

    memcpy( &( dr->getData().front() ), result.data(), result.size() );

    return dr;
  }

  // To load data from site

  return new WebSiteArticleRequest( urlString, netMgr, this );
}

class WebSiteResourceRequest: public WebSiteDataRequestSlots
{
  QNetworkReply * netReply;
  QString url;
  WebSiteDictionary * dictPtr;
  QNetworkAccessManager & mgr;

public:

  WebSiteResourceRequest( QString const & url_, QNetworkAccessManager & _mgr,
                          WebSiteDictionary * dictPtr_ );
  ~WebSiteResourceRequest()
  {}

  virtual void cancel();

private:

  virtual void requestFinished( QNetworkReply * );
};

WebSiteResourceRequest::WebSiteResourceRequest( QString const & url_,
                                                QNetworkAccessManager & _mgr,
                                                WebSiteDictionary * dictPtr_ ):
  url( url_ ), dictPtr( dictPtr_ ), mgr( _mgr )
{
  connect( &mgr, SIGNAL( finished( QNetworkReply * ) ),
           this, SLOT( requestFinished( QNetworkReply * ) ),
           Qt::QueuedConnection );

  QUrl reqUrl( url );

  netReply = mgr.get( QNetworkRequest( reqUrl ) );

#ifndef QT_NO_OPENSSL
  connect( netReply, SIGNAL( sslErrors( QList< QSslError > ) ),
           netReply, SLOT( ignoreSslErrors() ) );
#endif
}

void WebSiteResourceRequest::cancel()
{
  finish();
}

void WebSiteResourceRequest::requestFinished( QNetworkReply * r )
{
  if ( isFinished() ) // Was cancelled
    return;

  if ( r != netReply )
  {
    // Well, that's not our reply, don't do anything
    return;
  }

  if ( netReply->error() == QNetworkReply::NoError )
  {
    // Check for redirect reply

    QVariant possibleRedirectUrl = netReply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    QUrl redirectUrl = possibleRedirectUrl.toUrl();
    if( !redirectUrl.isEmpty() )
    {
      disconnect( netReply, 0, 0, 0 );
      netReply->deleteLater();
      netReply = mgr.get( QNetworkRequest( redirectUrl ) );
#ifndef QT_NO_OPENSSL
      connect( netReply, SIGNAL( sslErrors( QList< QSslError > ) ),
               netReply, SLOT( ignoreSslErrors() ) );
#endif
      return;
    }

    // Handle reply data

    QByteArray replyData = netReply->readAll();
    QString cssString = QString::fromUtf8( replyData );

    dictPtr->isolateWebCSS( cssString );

    QByteArray cssData = cssString.toUtf8();

    Mutex::Lock _( dataMutex );

    size_t prevSize = data.size();

    data.resize( prevSize + cssData.size() );

    memcpy( &data.front() + prevSize, cssData.data(), cssData.size() );

    hasAnyData = true;
  }
  else
    setErrorString( netReply->errorString() );

  disconnect( netReply, 0, 0, 0 );
  netReply->deleteLater();

  finish();
}

sptr< Dictionary::DataRequest > WebSiteDictionary::getResource( string const & name ) THROW_SPEC( std::exception )
{
  QString link = QString::fromUtf8( name.c_str() );
  int pos = link.indexOf( '/' );
  if( pos > 0 )
    link.replace( pos, 1, "://" );
  return new WebSiteResourceRequest( link, netMgr, this );
}

void WebSiteDictionary::loadIcon() throw()
{
  if ( dictionaryIconLoaded )
    return;

  if( !iconFilename.isEmpty() )
  {
    QFileInfo fInfo(  QDir( Config::getConfigDir() ), iconFilename );
    if( fInfo.isFile() )
      loadIconFromFile( fInfo.absoluteFilePath(), true );
  }
  if( dictionaryIcon.isNull() )
    dictionaryIcon = dictionaryNativeIcon = QIcon(":/icons/internet.png");
  dictionaryIconLoaded = true;
}

}

vector< sptr< Dictionary::Class > > makeDictionaries( Config::WebSites const & ws,
                                                      QNetworkAccessManager & mgr )
  THROW_SPEC( std::exception )
{
  vector< sptr< Dictionary::Class > > result;

  for( int x = 0; x < ws.size(); ++x )
  {
    if ( ws[ x ].enabled )
      result.push_back( new WebSiteDictionary( ws[ x ].id.toUtf8().data(),
                                               ws[ x ].name.toUtf8().data(),
                                               ws[ x ].url,
                                               ws[ x ].iconFilename,
                                               ws[ x ].inside_iframe,
                                               mgr )
                      );
  }

  return result;
}

}
