#if !defined(HISTORYSTATS_GUARD_COLUMN_PROTOCOL_H)
#define HISTORYSTATS_GUARD_COLUMN_PROTOCOL_H

#include "column.h"

/*
 * ColProtocol
 */

class ColProtocol
	: public Column
{
protected:
	virtual const mu_text* impl_getUID() const { return con::ColProtocol; }
	virtual const mu_text* impl_getTitle() const { return i18n(muT("Protocol")); }
	virtual const mu_text* impl_getDescription() const { return i18n(muT("Column holding the contact's protocol.")); }
	virtual int impl_getFeatures() const { return 0; }
	virtual int impl_configGetRestrictions(ext::string* pDetails) const { return crHTMLFull; }
	virtual void impl_outputRenderHeader(ext::ostream& tos, int row, int rowSpan) const;
	virtual void impl_outputRenderRow(ext::ostream& tos, const Contact& contact, DisplayType display);
};


#endif // HISTORYSTATS_GUARD_COLUMN_PROTOCOL_H