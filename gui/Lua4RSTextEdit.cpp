#include <QVariant>
#include <QEvent>
#include <QFontDatabase>

#include "Lua4RSTextEdit.h"

Lua4RSTextEdit::Lua4RSTextEdit(QWidget *parent) :
	QPlainTextEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _highlighter = new QLuaSyntaxHighlighter(document(), palette());
}

void Lua4RSTextEdit::changeEvent(QEvent* event)
{
    QPlainTextEdit::changeEvent(event);
    if(event->type() == QEvent::PaletteChange)
    {
        delete _highlighter;
        _highlighter = new QLuaSyntaxHighlighter(document(), palette());
        _highlighter->rehighlight();
    }
}

QLuaSyntaxHighlighter::QLuaSyntaxHighlighter(QTextDocument *document, const QPalette& palette) :
	QSyntaxHighlighter(document)
{
	HighlightingRule rule;
    const bool dark = palette.color(QPalette::Base).lightness() < 128;

	// special words
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(dark ? QColor("#93c5fd") : QColor("#1e40af"));
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns	<< "\\btrue\\b"		<< "\\bfalse\\b"	<< "\\bnil\\b"
					<< "\\bif\\b"		<< "\\bthen\\b"		<< "\\belse\\b"		<< "\\belseif\\b"
					<< "\\bfor\\b"		<< "\\bbreak\\b"	<< "\\bend\\b"
					<< "\\buntil\\b"	<< "\\bwhile\\b"	<< "\\bdo\\b"
					<< "\\bfunction\\b"	<< "\\bin\\b"		<< "\\blocal\\b"
					<< "\\brepeat\\b"	<< "\\breturn\\b";

	foreach (const QString &pattern, keywordPatterns) {
		rule.pattern = QRegExp(pattern);
		rule.format = keywordFormat;
		_highlightingRules.append(rule);
	}

	// operators
	QTextCharFormat operatorFormat;
	operatorFormat.setForeground(palette.color(QPalette::Text));
	operatorFormat.setFontWeight(QFont::Bold);
	QStringList operatorPatterns;
	operatorPatterns << "\\band\\b"	<< "\\bnot\\b" << "\\bor\\b"
					 << "\\+" << "\\-"
					 << "\\*" << "\\/"
					 << "\\<" << "\\<\\="
					 << "\\>" << "\\>\\="
					 << "\\=\\=" << "\\~\\=" << "\\="
					 << "\\^" << "\\.\\."
					 << "\\#";

	foreach (const QString &pattern, operatorPatterns) {
		rule.pattern = QRegExp(pattern);
		rule.format = operatorFormat;
		_highlightingRules.append(rule);
	}

	// numbers
	QTextCharFormat numberFormat;
	numberFormat.setForeground(dark ? QColor("#d8b4fe") : QColor("#7e22ce"));
	rule.pattern = QRegExp("[0-9]+");
	rule.format = numberFormat;
	_highlightingRules.append(rule);

	// functions
	QTextCharFormat functionFormat;
	functionFormat.setForeground(dark ? QColor("#67e8f9") : QColor("#0369a1"));
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	_highlightingRules.append(rule);

	// namespaces
	QTextCharFormat namespaceFormat;
	namespaceFormat.setForeground(dark ? QColor("#c4b5fd") : QColor("#6d28d9"));
	rule.pattern = QRegExp("\\b(?=(.*[a-z]))[A-Za-z0-9_]+(?=\\.)");
	rule.format = namespaceFormat;
	_highlightingRules.append(rule);

	// quotations - add at the end! (but before comments)
	QTextCharFormat quotationFormat;
	quotationFormat.setForeground(dark ? QColor("#fcd34d") : QColor("#854d0e"));
	rule.pattern = QRegExp("\".*\"");
	rule.pattern.setMinimal(true);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);
	rule.pattern = QRegExp("'.*'");
	rule.pattern.setMinimal(true);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);

	// comments - add at the end!
	// single line
	QTextCharFormat singleLineCommentFormat;
	singleLineCommentFormat.setForeground(dark ? QColor("#86efac") : QColor("#166534"));
	rule.pattern = QRegExp("--[^\n]*");
	rule.format = singleLineCommentFormat;
	_highlightingRules.append(rule);

	// multi lines
	_commentStartExpression = QRegExp("--\\[\\[");
	_commentEndExpression = QRegExp("\\]\\]");
	_multiLineCommentFormat.setForeground(dark ? QColor("#86efac") : QColor("#166534"));
	_multiLineCommentFormat.setFontItalic(true);

}

void QLuaSyntaxHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, _highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = _commentStartExpression.indexIn(text);

	while (startIndex >= 0) {
		int endIndex = _commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1) {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex
					+ _commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, _multiLineCommentFormat);
		startIndex = _commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
