#ifndef PAGEMETRICS_H
#define PAGEMETRICS_H

#include <QString>
#include <QSizeF>
#include <QMarginsF>
#include <QPageSize>

class QPaintDevice;


/**
 * @brief Page Metrics Class
 */
class PageMetrics
{
public:
	/**
	 * @brief Convert millimeters to pixels.
	 * @param _x indicates the direction (horizontal - true or vertical - false), in which
	 * must be calculated
	 */
	static qreal mmToPx(qreal _mm, bool _x = true);

	/**
	 * @brief Get page size from string
	 */
	static QPageSize::PageSizeId pageSizeIdFromString(const QString& _from);

	/**
	 * @brief Get a string of page size
	 */
	static QString stringFromPageSizeId(QPageSize::PageSizeId _pageSize);

public:
	PageMetrics(QPageSize::PageSizeId _pageFormat = QPageSize::A4, QMarginsF _mmMargins = QMarginsF());

	/**
	 * @brief Update Metrics
	 */
	void update(QPageSize::PageSizeId _pageFormat, QMarginsF _mmPageMargins = QMarginsF());

	/**
	 * @brief Methods of accessing page parameters
	 */
	/** @{ */
	QPageSize::PageSizeId pageFormat() const;
	QSizeF mmPageSize() const;
	QMarginsF mmPageMargins() const;
	QSizeF pxPageSize() const;
	QMarginsF pxPageMargins() const;
	/** @} */

private:
	/**
	 * @brief Page format
	 */
	QPageSize::PageSizeId m_pageFormat;

	/**
	 * @brief Dimensions in millimeters
	 */
	/** @{ */
	QSizeF m_mmPageSize;
	QMarginsF m_mmPageMargins;
	/** @} */

	/**
	 * @brief Dimensions in pixels
	 */
	/** @{ */
	QSizeF m_pxPageSize;
	QMarginsF m_pxPageMargins;
	/** @} */

};

#endif // PAGEMETRICS_H
