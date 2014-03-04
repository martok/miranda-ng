#if !defined(HISTORYSTATS_GUARD_COLUMN_WORDS_H)
#define HISTORYSTATS_GUARD_COLUMN_WORDS_H

#include "colbase_words.h"

/*
 * ColWords
 */

class ColWords
	: public ColBaseWords
{
public:
	typedef std::pair<InOut, ext::string> Word;
	typedef std::vector<Word> WordList;

private:
	class MostCommonWords
	{
	public:
		operator ()(const WordMap::value_type& a, const WordMap::value_type& b)
		{
			return (a.second != b.second) ? (a.second > b.second) : (a.first < b.first);
		}
	};

	class LeastCommonWords
	{
	public:
		operator ()(const WordMap::value_type& a, const WordMap::value_type& b)
		{
			return (a.second != b.second) ? (a.second < b.second) : (a.first < b.first);
		}
	};

	class LongestWords
	{
	public:
		operator ()(const WordMap::value_type& a, const WordMap::value_type& b)
		{
			return (a.first.length() != b.first.length()) ? (a.first.length() > b.first.length()) : (a.first < b.first);
		}
	};

protected:
	int m_nVisMode;
	int m_nNum;
	int m_nOffset;
	bool m_bDetail;
	bool m_bDetailInOut;
	bool m_bInOutColor;

	OptionsCtrl::Radio m_hVisMode;
	OptionsCtrl::Edit  m_hNum;
	OptionsCtrl::Edit  m_hOffset;
	OptionsCtrl::Check m_hDetail;
	OptionsCtrl::Check m_hDetailInOut;
	OptionsCtrl::Check m_hInOutColor;

private:
	void generateWords(WordMap* pWords, WordList* pWordList) const;

public:
	explicit ColWords();

protected:
	virtual const mu_text* impl_getUID() const { return con::ColWords; }
	virtual const mu_text* impl_getTitle() const { return i18n(muT("Words")); }
	virtual const mu_text* impl_getDescription() const { return i18n(muT("Column holding a list of a specified number of most/least common words or longest words used by you, by your contact, or by both of you.")); }
	virtual void impl_copyConfig(const Column* pSource);
	virtual void impl_configRead(const SettingsTree& settings);
	virtual void impl_configWrite(SettingsTree& settings) const;
	virtual void impl_configToUI(OptionsCtrl& Opt, OptionsCtrl::Item hGroup);
	virtual void impl_configFromUI(OptionsCtrl& Opt);
	virtual void impl_contactDataFree(Contact& contact) const;
	virtual void impl_contactDataTransform(Contact& contact) const;
	virtual void impl_contactDataTransformCleanup(Contact& contact) const;
	virtual StyleList impl_outputGetAdditionalStyles(IDProvider& idp);
	virtual void impl_outputRenderHeader(ext::ostream& tos, int row, int rowSpan) const;
	virtual void impl_outputRenderRow(ext::ostream& tos, const Contact& contact, DisplayType display);
};

#endif // HISTORYSTATS_GUARD_COLUMN_WORDS_H