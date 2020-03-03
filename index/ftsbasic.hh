//
// Created by joybin on 2020/3/1.
//

#ifndef GOLDENDICT_FTSBASIC_HH
#define GOLDENDICT_FTSBASIC_HH

namespace FTS {

    enum {
        // Minimum word length for indexing
        MinimumWordSize = 4,

        // Maximum dictionary size for first iteration of FTS indexing
        MaxDictionarySizeForFastSearch = 150000,

        // Maxumum match length for highlight search results
        // (QWebPage::findText() crashes on too long strings)
        MaxMatchLengthForHighlightResults = 500
    };

    enum SearchMode {
        WholeWords = 0,
        PlainText,
        Wildcards,
        RegExp
    };

    struct FtsHeadword
    {
        QString headword;
        QStringList dictIDs;
        QStringList foundHiliteRegExps;
        bool matchCase;

        FtsHeadword( QString const & headword_, QString const & dictid_,
                     QStringList hilites, bool match_case ) :
                headword( headword_ ),
                foundHiliteRegExps( hilites ),
                matchCase( match_case )
        {
            dictIDs.append( dictid_ );
        }

        QString trimQuotes( QString const & ) const;

        bool operator <( FtsHeadword const & other ) const;

        bool operator ==( FtsHeadword const & other ) const
        { return headword.compare( other.headword, Qt::CaseInsensitive ) == 0; }

        bool operator !=( FtsHeadword const & other ) const
        { return headword.compare( other.headword, Qt::CaseInsensitive ) != 0; }
    };
} // namespace FTS

#endif //GOLDENDICT_FTSBASIC_HH
