#include "PagesTextEdit.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QScrollBar>
#include <QTextFrame>

PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_document(0),
	m_usePageMode(false),
	m_addBottomSpace(true),
	m_showPageNumbers(true),
	m_pageNumbersAlignment(Qt::AlignTop | Qt::AlignRight)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	//
	// Configuring document change checking
	//
	aboutDocumentChanged();
	connect(this, SIGNAL(textChanged()), this, SLOT(aboutDocumentChanged()));

	//
	// Manual adjustment of the scroll interval
	//
	connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
		this, SLOT(aboutVerticalScrollRangeChanged(int,int)));
}

void PagesTextEdit::setPageFormat(QPageSize::PageSizeId _pageFormat)
{
	m_pageMetrics.update(_pageFormat);

	//
	// Redraw ourselves
	//
	repaint();
}

void PagesTextEdit::setPageMargins(const QMarginsF& _margins)
{
	m_pageMetrics.update(m_pageMetrics.pageFormat(), _margins);

	//
	// Redraw ourselves
	//
	repaint();
}

bool PagesTextEdit::usePageMode() const
{
	return m_usePageMode;
}

void PagesTextEdit::setUsePageMode(bool _use)
{
	if (m_usePageMode != _use) {
		m_usePageMode = _use;

		//
		// Redraw ourselves
		//
		repaint();
	}
}

void PagesTextEdit::setAddSpaceToBottom(bool _addSpace)
{
	if (m_addBottomSpace != _addSpace) {
		m_addBottomSpace = _addSpace;

		//
		// Redraw ourselves
		//
		repaint();
	}
}

void PagesTextEdit::setShowPageNumbers(bool _show)
{
	if (m_showPageNumbers != _show) {
		m_showPageNumbers = _show;

		//
		// Redraw ourselves
		//
		repaint();
	}
}

void PagesTextEdit::setPageNumbersAlignment(Qt::Alignment _align)
{
	if (m_pageNumbersAlignment != _align) {
		m_pageNumbersAlignment = _align;

		//
		// Redraw ourselves
		//
		repaint();
	}
}

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateVerticalScrollRange();

	paintPagesView();

	paintPageNumbers();

	QTextEdit::paintEvent(_event);
}

void PagesTextEdit::resizeEvent(QResizeEvent* _event)
{
	updateViewportMargins();

	updateVerticalScrollRange();

	QTextEdit::resizeEvent(_event);
}

void PagesTextEdit::updateViewportMargins()
{
	//
	// We form display parameters
	//
	QMargins viewportMargins;

	if (m_usePageMode) {
		//
		// Customize Document Size
		//

		int pageWidth = m_pageMetrics.pxPageSize().width();
		int pageHeight = m_pageMetrics.pxPageSize().height();

		//
		// Calculate indents for viewport
		//
		const int DEFAULT_TOP_MARGIN = 20;
		const int DEFAULT_BOTTOM_MARGIN = 20;
		{
			int leftMargin = 0;
			int rightMargin = 0;

			//
			// If the width of the editor is greater than the width of the page of the document, expand the side margins.
			//
			if (width() > pageWidth) {
				const int BORDERS_WIDTH = 4;
				const int VERTICAL_SCROLLBAR_WIDTH =
						verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
				// ... the width of the viewport frame and the editor itself
				leftMargin = rightMargin =
						(width() - pageWidth - VERTICAL_SCROLLBAR_WIDTH - BORDERS_WIDTH) / 2;
			}

			int topMargin = DEFAULT_TOP_MARGIN;

			//
			// The lower limit may be greater than the minimum value, for the case
			// when the whole document and even more fit on the screen
			//
			int bottomMargin = DEFAULT_BOTTOM_MARGIN;
			int documentHeight = pageHeight * document()->pageCount();
			if ((height() - documentHeight) > (DEFAULT_TOP_MARGIN + DEFAULT_BOTTOM_MARGIN)) {
				const int BORDERS_HEIGHT = 2;
				const int HORIZONTAL_SCROLLBAR_HEIGHT =
						horizontalScrollBar()->isVisible() ? horizontalScrollBar()->height() : 0;
				bottomMargin =
					height() - documentHeight - HORIZONTAL_SCROLLBAR_HEIGHT - DEFAULT_TOP_MARGIN - BORDERS_HEIGHT;
			}

			//
			// Adjust the indents themselves
			//
			viewportMargins = QMargins(leftMargin, topMargin, rightMargin, bottomMargin);
		}
	}

	setViewportMargins(viewportMargins);

	aboutUpdateDocumentGeometry();
}

void PagesTextEdit::updateVerticalScrollRange()
{
	//
	// In a page mode we show the page entirely
	//
	if (m_usePageMode) {

		const int pageHeight = m_pageMetrics.pxPageSize().height();
		const int documentHeight = pageHeight * document()->pageCount();
		const int maximumValue = documentHeight - viewport()->height();
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
	//
	// In normal mode, just add a little extra scrolling for convenience.
	//
	else {
		const int SCROLL_DELTA = 800;
		int maximumValue =
				document()->size().height() - viewport()->size().height()
				+ (m_addBottomSpace ? SCROLL_DELTA : 0);
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
}

void PagesTextEdit::paintPagesView()
{
	//
	// The design is drawn only when the editor is in page mode.
	//
	if (m_usePageMode) {
		//
		// Draw page breaks
		//

		qreal pageWidth = m_pageMetrics.pxPageSize().width();
		qreal pageHeight = m_pageMetrics.pxPageSize().height();

		QPainter p(viewport());
		QPen spacePen(palette().window(), 9);
		QPen borderPen(palette().dark(), 1);

		qreal curHeight = pageHeight - (verticalScrollBar()->value() % (int)pageHeight);
		//
		// Adjust the position of the right border
		//
		const int x = pageWidth + (width() % 2 == 0 ? 2 : 1);
		//
		// Horizontal offset if there is a scroll bar
		//
		const int horizontalDelta = horizontalScrollBar()->value();

		//
		// Draw the top border
		//
		if (curHeight - pageHeight >= 0) {
			p.setPen(borderPen);
			// ... top
			p.drawLine(0, curHeight - pageHeight, x, curHeight - pageHeight);
		}

		while (curHeight <= height()) {
			//
			// Page Break Background
			//
			p.setPen(spacePen);
			p.drawLine(0, curHeight-4, width(), curHeight-4);

			//
			// Page borders
			//
			p.setPen(borderPen);
			// ... lower
			p.drawLine(0, curHeight-8, x, curHeight-8);
			// ... top of next page
			p.drawLine(0, curHeight, x, curHeight);
			// ... left
			p.drawLine(0 - horizontalDelta, curHeight - pageHeight, 0 - horizontalDelta, curHeight - 8);
			// ... right
			p.drawLine(x - horizontalDelta, curHeight - pageHeight, x - horizontalDelta, curHeight - 8);

			curHeight += pageHeight;
		}

		//
		// Draw the side borders of the page when the page does not fit into the screen
		//
		if (curHeight >= height()) {
			//
			// Page borders
			//
			p.setPen(borderPen);
			// ... left
			p.drawLine(0 - horizontalDelta, curHeight-pageHeight, 0 - horizontalDelta, height());
			// ... right
			p.drawLine(x - horizontalDelta, curHeight-pageHeight, x - horizontalDelta, height());
		}
	}
}

void PagesTextEdit::paintPageNumbers()
{
	//
	// Page numbers are drawn only when the editor is in page mode,
	// if fields are set and the option to display numbers is enabled
	//
	if (m_usePageMode && !m_pageMetrics.pxPageMargins().isNull() && m_showPageNumbers) {
		//
		// Draw page numbers
		//

		QSizeF pageSize(m_pageMetrics.pxPageSize());
		QMarginsF pageMargins(m_pageMetrics.pxPageMargins());

		QPainter p(viewport());
		p.setFont(document()->defaultFont());
		p.setPen(QPen(palette().text(), 1));

		//
		// The current height and width are displayed on the screen.
		//
		qreal curHeight = pageSize.height() - (verticalScrollBar()->value() % (int)pageSize.height());

		//
		// The start of the field must take into account the scrollbar offset.
		//
		qreal leftMarginPosition = pageMargins.left() - horizontalScrollBar()->value();
		//
		// Total field width
		//
		qreal marginWidth = pageSize.width() - pageMargins.left() - pageMargins.right();

		//
		// The number of the first page to see.
		//
		int pageNumber = verticalScrollBar()->value() / pageSize.height() + 1;

		//
		// The top margin of the first page on the screen when the previous page is not visible.
		//
		if (curHeight - pageMargins.top() >= 0) {
			QRectF topMarginRect(leftMarginPosition, curHeight - pageSize.height(), marginWidth, pageMargins.top());
			paintPageNumber(&p, topMarginRect, true, pageNumber);
		}

		//
		// For all visible pages
		//
		while (curHeight < height()) {
			//
			// Define the bottom rectangle
			//
			QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
			paintPageNumber(&p, bottomMarginRect, false, pageNumber);

			//
			// Go to next page
			//
			++pageNumber;

			//
			// Define the top-field rectangle of the next page.
			//
			QRect topMarginRect(leftMarginPosition, curHeight, marginWidth, pageMargins.top());
			paintPageNumber(&p, topMarginRect, true, pageNumber);

			curHeight += pageSize.height();
		}
	}
}

void PagesTextEdit::paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number)
{
	//
	// Top field
	//
	if (_isHeader) {
		//
		// If the numbering is drawn in the top field
		//
		if (m_pageNumbersAlignment.testFlag(Qt::AlignTop)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignTop),
				QString::number(_number));
		}
	}
	//
	// Bottom field
	//
	else {
		//
		// If the numbering is drawn in the bottom field
		//
		if (m_pageNumbersAlignment.testFlag(Qt::AlignBottom)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignBottom),
				QString::number(_number));
		}
	}
}

void PagesTextEdit::aboutVerticalScrollRangeChanged(int _minimum, int _maximum)
{
	Q_UNUSED(_minimum);

	//
	// Update the viewport indents
	//
	updateViewportMargins();


	int scrollValue = verticalScrollBar()->value();

	//
	// If the current scroll position is greater than the maximum value,
	// then teksedit updated the interval itself, apply its own correction function
	//
	if (scrollValue > _maximum) {
		updateVerticalScrollRange();
	}
}

void PagesTextEdit::aboutDocumentChanged()
{
	if (m_document != document()) {
		m_document = document();

		//
		// Configuring the document size change check
		//
		connect(document()->documentLayout(), SIGNAL(update()), this, SLOT(aboutUpdateDocumentGeometry()));
	}
}

void PagesTextEdit::aboutUpdateDocumentGeometry()
{
	//
	// Determine the size of the document
	//
	QSizeF documentSize(width() - verticalScrollBar()->width(), -1);
	if (m_usePageMode) {
		int pageWidth = m_pageMetrics.pxPageSize().width();
		int pageHeight = m_pageMetrics.pxPageSize().height();
		documentSize = QSizeF(pageWidth, pageHeight);
	}

	//
	// Update document size
	//
	if (document()->pageSize() != documentSize) {
		document()->setPageSize(documentSize);
	}

	//
	// At the same time, set the indents
	//
	// ... remove the document
	//
	if (document()->documentMargin() != 0) {
		document()->setDocumentMargin(0);
	}
	//
	// ... and adjust the document fields
	//
	QMarginsF rootFrameMargins = m_pageMetrics.pxPageMargins();
	QTextFrameFormat rootFrameFormat = document()->rootFrame()->frameFormat();
	if (rootFrameFormat.leftMargin() != rootFrameMargins.left()
		|| rootFrameFormat.topMargin() != rootFrameMargins.top()
		|| rootFrameFormat.rightMargin() != rootFrameMargins.right()
		|| rootFrameFormat.bottomMargin() != rootFrameMargins.bottom()) {
		rootFrameFormat.setLeftMargin(rootFrameMargins.left());
		rootFrameFormat.setTopMargin(rootFrameMargins.top());
		rootFrameFormat.setRightMargin(rootFrameMargins.right());
		rootFrameFormat.setBottomMargin(rootFrameMargins.bottom());
		document()->rootFrame()->setFrameFormat(rootFrameFormat);
	}
}

