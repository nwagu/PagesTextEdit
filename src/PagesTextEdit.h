#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

#include "PageMetrics.h"


/**
 * @brief text editor with the ability to paginate content
 */
class PagesTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit PagesTextEdit(QWidget* parent = 0);

	/**
	 * @brief Set page format
	 */
	void setPageFormat(QPageSize::PageSizeId _pageFormat);

	/**
	 * @brief Customize page margins
	 */
	void setPageMargins(const QMarginsF& _margins);

	/**
	 * @brief Get text display mode
	 */
	bool usePageMode() const;

public slots:
	/**
	 * @brief Set text display mode
	 */
	void setUsePageMode(bool _use);

	/**
	 * @brief Set the value of the need to add an extra scroll down
	 */
	void setAddSpaceToBottom(bool _addSpace);

	/**
	 * @brief Set the value of the need to display page numbers
	 */
	void setShowPageNumbers(bool _show);

	/**
	 * @brief Set the display location of page numbers
	 */
	void setPageNumbersAlignment(Qt::Alignment _align);

protected:
	/**
	 * @brief Redefined to correct the document and draw the page layout.
	 */
	void paintEvent(QPaintEvent* _event);

	/**
	 * @brief Overridden for correct update of viewport size
	 */
	void resizeEvent(QResizeEvent* _event);

private:
	/**
	 * @brief Update viewport indents
	 */
	void updateViewportMargins();

	/**
	 * @brief Refresh vertical scroll interval
	 */
	void updateVerticalScrollRange();

	/**
	 * @brief Draw the design of the document pages
	 */
	void paintPagesView();

	/**
	 * @brief Draw page numbers
	 */
	void paintPageNumbers();

	/**
	 * @brief Draw the page number with the given location parameters.
	 */
	void paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number);

private slots:
	/**
	 * @brief The vertical scroll interval has changed.
	 */
	void aboutVerticalScrollRangeChanged(int _minimum, int _maximum);

	/**
	 * @brief Check document change
	 */
	void aboutDocumentChanged();

	/**
	 * @brief Update Document Geometry
	 * @note The standard implementation of QTextEdit is such that it always resets the installed
	 *		 document size resulting in undesirable consequences
	 */
	void aboutUpdateDocumentGeometry();

private:
	/**
	 * @brief Link to current document
	 */
	QTextDocument* m_document;

	/**
	 * @brief Text display mode
	 *
	 * true - page by page
	 * false - solid
	 */
	bool m_usePageMode;

	/**
	 * @brief Do you need to add space below in normal mode
	 */
	bool m_addBottomSpace;

	/**
	 * @brief Do I need to show page numbers
	 */
	bool m_showPageNumbers;

	/**
	 * @brief Where to show page numbers
	 */
	Qt::Alignment m_pageNumbersAlignment;

	/**
	 * @brief Editor page metrics
	 */
	PageMetrics m_pageMetrics;
};

#endif // PAGESTEXTEDIT_H
