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
	// 禁用Insert按键的功能
	this->clearCmdKey(SCK_INSERT);

	for (int i = SC_MARKNUM_FOLDEREND; i <= SC_MARKNUM_FOLDEROPEN; ++i)
	{
		// 设置前景色、背景色、选中的背景色
		this->markerSetFore(i, 0xF3F3F3);
		this->markerSetBack(i, 0x808080);
		this->markerSetBackSelected(i, 0x0000FF);
	}

	// 控制语法高亮样式化行为
	this->setIdleStyling(SC_IDLESTYLING_TOVISIBLE);
	// 控制编辑器的滚动行为
	this->setEndAtLastLine(false);

	// 启用或禁用多重选择功能
	this->setMultipleSelection(true);
	// 多重选择模式下，输入、光标移动、删除等操作能够同时应用到所有选中的区域
	this->setAdditionalSelectionTyping(true);
	// 多重选择模式下，粘贴操作的行为
	this->setMultiPaste(SC_MULTIPASTE_EACH);
	// 控制虚拟空间的行为
	this->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);

	// 设置文本区域左侧空白边距的宽度，单位是像素
	this->setMarginLeft(2);

	// 设置指定边距（margin）的宽度
	this->setMarginWidthN(0, 30);
	// 设置特定边距的掩码
	this->setMarginMaskN(1, (1 << MARK_HIDELINESBEGIN) | (1 << MARK_HIDELINESEND) | (1 << MARK_HIDELINESUNDERLINE));
	this->setMarginMaskN(2, SC_MASK_FOLDERS);
	this->setMarginWidthN(2, 14);

	// 在 Scintilla 编辑器中定义标记符号
	this->markerDefine(MARK_HIDELINESUNDERLINE, SC_MARK_UNDERLINE);
	this->markerSetBack(MARK_HIDELINESUNDERLINE, 0x77CC77);

	this->markerDefine(MARK_HIDELINESBEGIN, SC_MARK_ARROW);
	this->markerDefine(MARK_HIDELINESEND, SC_MARK_ARROWDOWN);

	// 设置编辑器的边距（Margin）是否响应鼠标点击的消息
	this->setMarginSensitiveN(2, true);

	// 控制代码折叠（folding）功能的显示方式
	this->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
	// 用于控制滚动宽度跟踪
	this->setScrollWidthTracking(true);
	// 设置 Scintilla 假设的文档宽度
	this->setScrollWidth(1);

	// 设置制表符（Tab）字符的绘制方式
	this->setTabDrawMode(SCTD_STRIKEOUT);
	// 设置制表符宽度
	this->setTabWidth(4);
	// 用于控制退格键在缩进区域内的行为
	this->setBackSpaceUnIndents(true);

	// 设置当前光标所在行的背景颜色
	this->setCaretLineVisible(true);
	// 设置光标所在行的背景色始终可见
	this->setCaretLineVisibleAlways(true);
	// 设置光标的宽度
	this->setCaretWidth(2);

	// 设置超出指定列数的行所标记的颜色
	this->setEdgeColour(0x80FFFF);

	// https://www.scintilla.org/ScintillaDoc.html#ElementColours
	// SC_ELEMENT_SELECTION_TEXT
	// SC_ELEMENT_SELECTION_BACK
	// SC_ELEMENT_SELECTION_ADDITIONAL_TEXT
	// SC_ELEMENT_SELECTION_ADDITIONAL_BACK
	// SC_ELEMENT_SELECTION_SECONDARY_TEXT
	// SC_ELEMENT_SELECTION_SECONDARY_BACK
	// SC_ELEMENT_SELECTION_INACTIVE_TEXT
	// 用于定义窗口失去键盘焦点时的选择（文本高亮）颜色
	this->setElementColour(SC_ELEMENT_SELECTION_INACTIVE_BACK, 0xFFE0E0E0);
	// SC_ELEMENT_CARET
	// SC_ELEMENT_CARET_ADDITIONAL
	// 通过设置 `SC_ELEMENT_CARET_LINE_BACK` 元素来改变光标行的背景颜色
	this->setElementColour(SC_ELEMENT_CARET_LINE_BACK, 0xFFFFE8E8);
	// 通过设置 `SC_ELEMENT_WHITE_SPACE` 元素来改变空白字符的前景色
	this->setElementColour(SC_ELEMENT_WHITE_SPACE, 0xFFD0D0D0);
	// SC_ELEMENT_WHITE_SPACE_BACK
	// SC_ELEMENT_HOT_SPOT_ACTIVE
	// SC_ELEMENT_HOT_SPOT_ACTIVE_BACK
	// 通过设置 `SC_ELEMENT_FOLD_LINE` 元素来改变折叠线的颜色
	this->setElementColour(SC_ELEMENT_FOLD_LINE, 0xFFA0A0A0);
	// SC_ELEMENT_HIDDEN_LINE

	// 设置和空白字符的大小
	this->setWhitespaceSize(2);

	// 用于设置折叠边距和折叠边距高亮的颜色
	this->setFoldMarginColour(true, 0xFFFFFF);
	this->setFoldMarginHiColour(true, 0xE9E9E9);

	// 用于设置自动折叠的行为
	this->setAutomaticFold(SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CLICK | SC_AUTOMATICFOLD_CHANGE);
	// 用于控制是否启用对当前折叠块的高亮显示
	this->markerEnableHighlight(true);

	// 用于将编辑器中的字符集设置为默认的空白字符和单词字符集
	this->setCharsDefault();
	// 定义哪些字符属于“单词”类别
	this->setWordChars(this->wordChars() + QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_").toLatin1());

	// 设置指定样式的文本前景颜色-默认样式
	this->styleSetFore(STYLE_DEFAULT, 0x000000);
	// 设置指定样式的文本背景颜色-默认样式
	this->styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
	this->styleSetSize(STYLE_DEFAULT,/* m_settings->fontSize()*/12);
	this->styleSetFont(STYLE_DEFAULT, /*m_settings->fontName().toUtf8().data()*/"新宋体");
	// 将所有样式的设置清除，并将它们的属性设置为 `STYLE_DEFAULT` 的属性
	this->styleClearAll();

	// 用于显示行号边栏中的文本，定义了行号的字体、颜色等属性
	this->styleSetFore(STYLE_LINENUMBER, 0x808080);
	this->styleSetBack(STYLE_LINENUMBER, 0xE4E4E4);
	this->styleSetBold(STYLE_LINENUMBER, false);

	// 用于高亮显示配对的括号
	this->styleSetFore(STYLE_BRACELIGHT, 0x0000FF);
	this->styleSetBack(STYLE_BRACELIGHT, 0xFFFFFF);

	// 当一个括号没有配对时，此样式会标记为不匹配的括号
	this->styleSetFore(STYLE_BRACEBAD, 0x000080);
	this->styleSetBack(STYLE_BRACEBAD, 0xFFFFFF);

	// 定义了缩进指南的前景色和背景色
	this->styleSetFore(STYLE_INDENTGUIDE, 0xC0C0C0);
	this->styleSetBack(STYLE_INDENTGUIDE, 0xFFFFFF);

	// STYLE_CONTROLCHAR
	// STYLE_CALLTIP
	// STYLE_FOLDDISPLAYTEXT

	// 设置如何显示空白字符的方式
	this->setViewWS(/*m_settings->showWhitespace() ? SCWS_VISIBLEALWAYS :*/ SCWS_INVISIBLE);
	// 用于控制行结束符（End of Line, EOL）的显示方式
	this->setViewEOL(/*m_settings->showEndOfLine()*/false);
	// 用于控制文本行换行时显示的可视化标记
	this->setWrapVisualFlags(/*m_settings->showWrapSymbol() ? SC_WRAPVISUALFLAG_END : */SC_WRAPVISUALFLAG_NONE);
	// 设置缩进引导线（Indentation Guides）显示方式的消息
	this->setIndentationGuides(/*m_settings->showIndentGuide() ? SC_IV_LOOKBOTH : */SC_IV_NONE);
	// 设置文本框的换行模式
	this->setWrapMode(/*m_settings->wordWrap() ? SC_WRAP_WORD : */SC_WRAP_NONE);
}