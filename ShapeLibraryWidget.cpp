#include <ShapeLibraryWidget.h>
#include <QGroupBox>
#include <QPushButton>
#include <QDrag>               // 提供支持拖放操作的功能
#include <QMimeData>           // 提供数据传输功能,用于在拖放操作和剪贴板之间传输数据
#include <QMouseEvent>
#include <QIcon>

ShapeLibraryWidget::ShapeLibraryWidget(QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(false);      // 设置接受拖放操作

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(20);
	mainLayout->addSpacing(10);
	mainLayout->setContentsMargins(10, 10, 10, 10);

	// 基础图形组合框
	QGroupBox* basicGroup = new QGroupBox(tr("Basic Shapes"));
	basicGroup->setStyleSheet(groupBoxStyle);
	FlowLayout* basicLayout = new FlowLayout(basicGroup);
	basicLayout->setSpacing(10);
	basicLayout->setContentsMargins(10, 10, 10, 10);

	m_iconShapeMap.insert(":/images/rect.png", "Rect");
	m_iconShapeMap.insert(":/images/ellipse.png", "Ellipse");
	m_iconShapeMap.insert(":/images/diamond.png", "Diamond");
	m_iconShapeMap.insert(":/images/pentagon.png", "Pentagon");
	m_iconShapeMap.insert(":/images/hexagon.png", "Hexagon");
	m_iconShapeMap.insert(":/images/arrow.png", "Arrow");
	m_iconShapeMap.insert(":/images/line.png", "Line");
	setupButtons(basicLayout, m_iconShapeMap);
	mainLayout->addWidget(basicGroup);

	// UML图元组合框
	QGroupBox* umlGroup = new QGroupBox(tr("UML"));
	umlGroup->setStyleSheet(groupBoxStyle);
	FlowLayout* umlLayout = new FlowLayout(umlGroup);
	umlLayout->setSpacing(5);
	umlLayout->setContentsMargins(5, 5, 5, 5);

	mainLayout->addWidget(umlGroup);
	mainLayout->addStretch();
	setLayout(mainLayout);
}

void ShapeLibraryWidget::setupButtons(FlowLayout* layout, QMap<QString, QString>& iconMap)
{
    // 遍历iconMap中的每个键值对，为每个条目创建一个按钮
	for (auto it = iconMap.constBegin(); it != iconMap.constEnd(); ++it)
	{
		QPushButton* btn = new QPushButton();
		btn->setIcon(QIcon(it.key()));
		btn->setIconSize(QSize(36, 36));
		btn->setObjectName(it.value());
		btn->setFixedSize(48, 48);
		btn->setStyleSheet("QPushButton { border:none; background: transparent; }"
			"QPushButton:hover { background-color:rgb(255,255,255,100); }");

		// 拖动事件
		connect(btn, &QPushButton::pressed, this, [=]()
			{
				QMimeData* mimeData = new QMimeData;
				mimeData->setText(btn->objectName());                // 存储按钮对象名称

				QDrag* drag = new QDrag(this);
				drag->setMimeData(mimeData);
				drag->setPixmap(btn->icon().pixmap(36, 36));         // 设置拖动时显示的图标
				drag->exec(Qt::CopyAction);
			});
		layout->addWidget(btn);
	}
}