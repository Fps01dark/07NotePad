#include "custom_text_edit.h"

#include "framework.h"
#include "message_bus.h"

namespace
{
	const int MARK_HIDELINESBEGIN = 23;
	const int MARK_HIDELINESEND = 22;
	const int MARK_HIDELINESUNDERLINE = 21;
}

CustomTextEdit::CustomTextEdit(std::shared_ptr<MessageBus> message_bus, QWidget* parent)
	: m_messageBus(message_bus), ScintillaEdit(parent)
{
	InitUi();
	InitValue();
	InitConnect();
}

CustomTextEdit::~CustomTextEdit()
{
}

void CustomTextEdit::SetText(const QString& text)
{
	setText(text.toUtf8().constData());
}

QString CustomTextEdit::GetText()
{
	return  QString::fromUtf8(getText(length()));
}

void CustomTextEdit::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		int   delta = event->angleDelta().y();
		QFont font = this->font();
		int   current_size = font.pointSize();
		if (delta > 0)
		{
			m_messageBus->Publish("Change Zoom", current_size + 1);
		}
		else
		{
			m_messageBus->Publish("Change Zoom", current_size - 1);
		}
	}
	else {
		ScintillaEdit::wheelEvent(event);  // 非Ctrl键盘缩放时正常处理
	}
}

void CustomTextEdit::InitUi()
{
	SetupEditor();
}

void CustomTextEdit::InitValue()
{
}

void CustomTextEdit::InitConnect() {}

void CustomTextEdit::SetupEditor()
{
	this->clearCmdKey(SCK_INSERT);

	for (int i = SC_MARKNUM_FOLDEREND; i <= SC_MARKNUM_FOLDEROPEN; ++i)
	{
		this->markerSetFore(i, 0xF3F3F3);
		this->markerSetBack(i, 0x808080);
		this->markerSetBackSelected(i, 0x0000FF);
	}

	this->setIdleStyling(SC_IDLESTYLING_TOVISIBLE);
	this->setEndAtLastLine(false);

	this->setMultipleSelection(true);
	this->setAdditionalSelectionTyping(true);
	this->setMultiPaste(SC_MULTIPASTE_EACH);
	this->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);

	this->setMarginLeft(2);

	this->setMarginWidthN(0, 30);
	this->setMarginMaskN(1, (1 << MARK_HIDELINESBEGIN) | (1 << MARK_HIDELINESEND) | (1 << MARK_HIDELINESUNDERLINE));
	this->setMarginMaskN(2, SC_MASK_FOLDERS);
	this->setMarginWidthN(2, 14);

	this->markerDefine(MARK_HIDELINESUNDERLINE, SC_MARK_UNDERLINE);
	this->markerSetBack(MARK_HIDELINESUNDERLINE, 0x77CC77);

	this->markerDefine(MARK_HIDELINESBEGIN, SC_MARK_ARROW);
	this->markerDefine(MARK_HIDELINESEND, SC_MARK_ARROWDOWN);

	this->setMarginSensitiveN(2, true);

	this->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
	this->setScrollWidthTracking(true);
	this->setScrollWidth(1);

	this->setTabDrawMode(SCTD_STRIKEOUT);
	this->setTabWidth(4);
	this->setBackSpaceUnIndents(true);

	this->setCaretLineVisible(true);
	this->setCaretLineVisibleAlways(true);
	this->setCaretWidth(2);

	this->setEdgeColour(0x80FFFF);

	// https://www.scintilla.org/ScintillaDoc.html#ElementColours
	// SC_ELEMENT_SELECTION_TEXT
	// SC_ELEMENT_SELECTION_BACK
	// SC_ELEMENT_SELECTION_ADDITIONAL_TEXT
	// SC_ELEMENT_SELECTION_ADDITIONAL_BACK
	// SC_ELEMENT_SELECTION_SECONDARY_TEXT
	// SC_ELEMENT_SELECTION_SECONDARY_BACK
	// SC_ELEMENT_SELECTION_INACTIVE_TEXT
	this->setElementColour(SC_ELEMENT_SELECTION_INACTIVE_BACK, 0xFFE0E0E0);
	// SC_ELEMENT_CARET
	// SC_ELEMENT_CARET_ADDITIONAL
	this->setElementColour(SC_ELEMENT_CARET_LINE_BACK, 0xFFFFE8E8);
	this->setElementColour(SC_ELEMENT_WHITE_SPACE, 0xFFD0D0D0);
	// SC_ELEMENT_WHITE_SPACE_BACK
	// SC_ELEMENT_HOT_SPOT_ACTIVE
	// SC_ELEMENT_HOT_SPOT_ACTIVE_BACK
	this->setElementColour(SC_ELEMENT_FOLD_LINE, 0xFFA0A0A0);
	// SC_ELEMENT_HIDDEN_LINE

	this->setWhitespaceSize(2);

	this->setFoldMarginColour(true, 0xFFFFFF);
	this->setFoldMarginHiColour(true, 0xE9E9E9);

	this->setAutomaticFold(SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CLICK | SC_AUTOMATICFOLD_CHANGE);
	this->markerEnableHighlight(true);

	this->setCharsDefault();
	this->setWordChars(this->wordChars()/* + m_settings->additionalWordChars().toLatin1()*/);

	this->styleSetFore(STYLE_DEFAULT, 0x000000);
	this->styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
	this->styleSetSize(STYLE_DEFAULT,/* m_settings->fontSize()*/12);
	this->styleSetFont(STYLE_DEFAULT, /*m_settings->fontName().toUtf8().data()*/"新宋体");
	this->styleClearAll();

	this->styleSetFore(STYLE_LINENUMBER, 0x808080);
	this->styleSetBack(STYLE_LINENUMBER, 0xE4E4E4);
	this->styleSetBold(STYLE_LINENUMBER, false);

	this->styleSetFore(STYLE_BRACELIGHT, 0x0000FF);
	this->styleSetBack(STYLE_BRACELIGHT, 0xFFFFFF);

	this->styleSetFore(STYLE_BRACEBAD, 0x000080);
	this->styleSetBack(STYLE_BRACEBAD, 0xFFFFFF);

	this->styleSetFore(STYLE_INDENTGUIDE, 0xC0C0C0);
	this->styleSetBack(STYLE_INDENTGUIDE, 0xFFFFFF);

	// STYLE_CONTROLCHAR
	// STYLE_CALLTIP
	// STYLE_FOLDDISPLAYTEXT

	this->setViewWS(/*m_settings->showWhitespace() ? */SCWS_VISIBLEALWAYS/* : SCWS_INVISIBLE*/);
	this->setViewEOL(/*m_settings->showEndOfLine()*/true);
	this->setWrapVisualFlags(/*m_settings->showWrapSymbol() ? */SC_WRAPVISUALFLAG_END /*: SC_WRAPVISUALFLAG_NONE*/);
	this->setIndentationGuides(/*m_settings->showIndentGuide() ? */SC_IV_LOOKBOTH /*: SC_IV_NONE*/);
	this->setWrapMode(/*m_settings->wordWrap() ? */SC_WRAP_WORD/* : SC_WRAP_NONE*/);
}