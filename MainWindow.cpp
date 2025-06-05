#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("Flowchart Drawing Tool"));
	resize(1200, 800);

	setStyleSheet("QMainWindow { background-color:  #F1F3F4; }");

	// 绘图区域
	drawArea = new DrawArea(this);
	QScrollArea* drawScroll = new QScrollArea(this);
	drawScroll->setWidget(drawArea);
	drawScroll->setWidgetResizable(true);
	drawScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	drawScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(drawScroll);

	// 菜单栏和工具栏
	setupMenuBar();
	setupToolBar();

	// 图形库和属性面板
	setupDockWidgets();

	propertyPanel->initWithDrawArea(drawArea);

	// 工具栏和状态栏相关信号槽连接
	connect(newFileAction, &QAction::triggered, drawArea, &DrawArea::newFile);
	connect(openFileAction, &QAction::triggered, drawArea, &DrawArea::openFile);
	connect(saveFileAction, &QAction::triggered, drawArea, &DrawArea::saveFile);
	connect(saveAsFileAction, &QAction::triggered, drawArea, &DrawArea::saveAsFile);
	connect(exportPngAction, &QAction::triggered, drawArea, &DrawArea::exportToPng);
	connect(exportSvgAction, &QAction::triggered, drawArea, &DrawArea::exportToSvg);

	connect(moveTopAction, &QAction::triggered, drawArea, &DrawArea::moveSelectedShapeToTop);
	connect(moveBottomAction, &QAction::triggered, drawArea, &DrawArea::moveSelectedShapeToBottom);
	connect(moveUpAction, &QAction::triggered, drawArea, &DrawArea::moveSelectedShapeUp);
	connect(moveDownAction, &QAction::triggered, drawArea, &DrawArea::moveSelectedShapeDown);
    // 图形顺序改变时更新工具栏和状态栏
	connect(drawArea, &DrawArea::shapeOrderChanged, this, &MainWindow::updateActions);

	connect(undoAction, &QAction::triggered, drawArea, &DrawArea::undo);
	connect(redoAction, &QAction::triggered, drawArea, &DrawArea::redo);
	connect(copyAction, &QAction::triggered, drawArea, &DrawArea::copySelectedShape);
	connect(pasteAction, &QAction::triggered, this, [this]() {
		QPointF pastePos = QPointF(500, 500);
		drawArea->pasteShape(pastePos);
		});
	connect(cutAction, &QAction::triggered, drawArea, &DrawArea::cutSelectedShape);
	connect(deleteAction, &QAction::triggered, drawArea, &DrawArea::deleteSelectedShape);
	connect(selectAllAction, &QAction::triggered, drawArea, &DrawArea::selectAll);
	connect(drawArea, &DrawArea::undoStateChanged, this, &MainWindow::updateActions);
	connect(drawArea, &DrawArea::redoStateChanged, this, &MainWindow::updateActions);
	connect(drawArea, &DrawArea::deleteSelectedShapeChanged, this, &MainWindow::updateActions);
	connect(drawArea, &DrawArea::selectionChanged, this, &MainWindow::updateActions);
	updateActions();


	// 页面相关信号槽连接
	connect(propertyPanel, &PropertyPanel::backgroundColorChanged, drawArea, &DrawArea::setCurrentBackgroundColor);

	propertyPanel->gridCheckBox->setChecked(drawArea->getGridVisible());
	connect(propertyPanel->gridCheckBox, &QCheckBox::toggled, drawArea, &DrawArea::setGridVisible);
	connect(drawArea, &DrawArea::gridVisibilityChanged, propertyPanel->gridCheckBox, &QCheckBox::setChecked);

	connect(propertyPanel, &PropertyPanel::pageSizeChanged, drawArea, &DrawArea::setPageSize);
	connect(propertyPanel, &PropertyPanel::pageOrientationChanged, drawArea, [this](bool landscape) {
		QSize size = drawArea->getPageSizeInPixels();

		if (landscape != (size.width() > size.height())) {
			size.transpose();
			drawArea->blockSignals(true);
			drawArea->setPageSize(size);
			drawArea->blockSignals(false);

			// 同步到属性面板
			propertyPanel->widthSpinBox->blockSignals(true);
			propertyPanel->widthSpinBox->setValue(size.width());
			propertyPanel->widthSpinBox->blockSignals(false);
			propertyPanel->heightSpinBox->blockSignals(true);
			propertyPanel->heightSpinBox->setValue(size.height());
			propertyPanel->heightSpinBox->blockSignals(false);
		}

		drawArea->setPageOrientation(landscape);
		});
	connect(propertyPanel, &PropertyPanel::pageWidthChanged, drawArea, [this](int w) {
		QSize size = drawArea->getPageSizeInPixels();
		size.setWidth(w);
		drawArea->blockSignals(true);
		drawArea->setPageSize(size);
		drawArea->blockSignals(false);
		});
	connect(propertyPanel, &PropertyPanel::pageHeightChanged, drawArea, [this](int h) {
		QSize size = drawArea->getPageSizeInPixels();
		size.setHeight(h);
		drawArea->blockSignals(true);
		drawArea->setPageSize(size);
		drawArea->blockSignals(false);
		});
	connect(drawArea, &DrawArea::pageSizeChanged, this, [=](const QSize& size) {
		propertyPanel->widthSpinBox->blockSignals(true);
		propertyPanel->widthSpinBox->setValue(size.width());
		propertyPanel->widthSpinBox->blockSignals(false);
		propertyPanel->heightSpinBox->blockSignals(true);
		propertyPanel->heightSpinBox->setValue(size.height());
		propertyPanel->heightSpinBox->blockSignals(false);
		});
	connect(drawArea, &DrawArea::pageOrientationChanged, [this](bool landscape) {
		propertyPanel->orientationGroup->button(landscape ? 1 : 0)->setChecked(true);
		});

	// 图形相关信号槽连接
	// 边框
	connect(propertyPanel, &PropertyPanel::borderColorChanged, this, [this](const QColor& color) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setBorderColor(color);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::borderWidthChanged, this, [this](int width) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setPenWidth(width);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::borderStyleChanged, this, [this](Qt::PenStyle style) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setBorderStyle(style);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::fillColorChanged, this, [this](const QColor& color) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFillColor(color);
			drawArea->update();
		}
		});

	// 文本
	connect(propertyPanel, &PropertyPanel::fontColorChanged, this, [this](const QColor& color) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontColor(color);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::fontSizeChanged, this, [this](int size) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontSize(size);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::fontFamilyChanged, this, [this](const QString& family) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontFamily(family);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::textBoldChanged, this, [this](bool bold) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontBold(bold);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::textItalicChanged, this, [this](bool italic) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontItalic(italic);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::textUnderlineChanged, this, [this](bool underline) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setFontUnderline(underline);
			drawArea->update();
		}
		});

	connect(propertyPanel, &PropertyPanel::textAlignmentChanged, this, [this](Qt::Alignment alignment) {
		if (auto shape = drawArea->getSelectedShape()) {
			shape->setTextAlignment(alignment);
			drawArea->update();
		}
		});

	// 调整图形
	connect(propertyPanel->moveTopButton, &QPushButton::clicked, drawArea, &DrawArea::moveSelectedShapeToTop);
	connect(propertyPanel->moveBottomButton, &QPushButton::clicked, drawArea, &DrawArea::moveSelectedShapeToBottom);
	connect(propertyPanel->moveUpButton, &QPushButton::clicked, drawArea, &DrawArea::moveSelectedShapeUp);
	connect(propertyPanel->moveDownButton, &QPushButton::clicked, drawArea, &DrawArea::moveSelectedShapeDown);

    // 根据是否有图形被选中，更新属性面板状态
	connect(drawArea, &DrawArea::selectionChanged, propertyPanel, &PropertyPanel::updatePanel);
}

MainWindow::~MainWindow() {}

void MainWindow::setupMenuBar()
{
	// 文件
	menuBar()->setStyleSheet(menuBarStyle);
	QMenu* fileMenu = menuBar()->addMenu(tr("File"));
	newFileAction = fileMenu->addAction(tr("New"));
	openFileAction = fileMenu->addAction(tr("Open"));
	saveFileAction = fileMenu->addAction(tr("Save"));
	saveAsFileAction = fileMenu->addAction(tr("Save As"));   // 另存为
	exportPngAction = fileMenu->addAction(tr("Export PNG"));
	exportSvgAction = fileMenu->addAction(tr("Export SVG"));
	newFileAction->setIcon(QIcon(":/images/new.png"));
	openFileAction->setIcon(QIcon(":/images/open.png"));
	saveFileAction->setIcon(QIcon(":/images/save.png"));
	saveAsFileAction->setIcon(QIcon(":/images/save-as.png"));
	exportPngAction->setIcon(QIcon(":/images/export-png.png"));
	exportSvgAction->setIcon(QIcon(":/images/export-svg.png"));

	// 编辑
	QMenu* editMenu = menuBar()->addMenu(tr("Edit"));
	undoAction = editMenu->addAction(tr("Undo"));
	redoAction = editMenu->addAction(tr("Redo"));
	copyAction = editMenu->addAction(tr("Copy"));
	pasteAction = editMenu->addAction(tr("Paste"));
	cutAction = editMenu->addAction(tr("Cut"));
	deleteAction = editMenu->addAction(tr("Delete"));
	selectAllAction = editMenu->addAction(tr("Select All"));
	undoAction->setIcon(QIcon(":/images/undo.png"));
	redoAction->setIcon(QIcon(":/images/redo.png"));
	copyAction->setIcon(QIcon(":/images/copy.png"));
	pasteAction->setIcon(QIcon(":/images/paste.png"));
	cutAction->setIcon(QIcon(":/images/cut.png"));
	deleteAction->setIcon(QIcon(":/images/delete.png"));
	selectAllAction->setIcon(QIcon(":/images/selectall.png"));
	undoAction->setShortcut(QKeySequence::Undo);
	redoAction->setShortcut(QKeySequence::Redo);
	copyAction->setShortcut(QKeySequence::Copy);
	pasteAction->setShortcut(QKeySequence::Paste);
	cutAction->setShortcut(QKeySequence::Cut);
	deleteAction->setShortcut(QKeySequence::Delete);
	selectAllAction->setShortcut(QKeySequence::SelectAll);

	// 画布
	QMenu* viewMenu = menuBar()->addMenu(tr("Canvas"));
	QAction* gridVisibleAction = viewMenu->addAction(tr("Grid"));
	gridVisibleAction->setCheckable(true);

	gridVisibleAction->setChecked(drawArea->getGridVisible());
	connect(gridVisibleAction, &QAction::triggered, drawArea, &DrawArea::setGridVisible);
	connect(drawArea, &DrawArea::gridVisibilityChanged, gridVisibleAction, &QAction::setChecked);

	// 排列
	QMenu* arrangeMenu = menuBar()->addMenu(tr("Arrange"));
	moveTopAction = new QAction(tr("Move Top"));
	moveBottomAction = new QAction(tr("Move Bottom"));
	moveUpAction = new QAction(tr("Move Up"));
	moveDownAction = new QAction(tr("Move Down"));
	moveTopAction->setIcon(QIcon(":/images/move-top.png"));
	moveBottomAction->setIcon(QIcon(":/images/move-bottom.png"));
	moveUpAction->setIcon(QIcon(":/images/move-up.png"));
	moveDownAction->setIcon(QIcon(":/images/move-down.png"));

	arrangeMenu->addAction(moveTopAction);
	arrangeMenu->addAction(moveBottomAction);
	arrangeMenu->addAction(moveUpAction);
	arrangeMenu->addAction(moveDownAction);

	menuBar()->addMenu(tr("Other"));
	menuBar()->addMenu(tr("Help"));
}

void MainWindow::setupToolBar()
{
	QToolBar* toolBar = addToolBar(tr("Tools"));
	toolBar->addAction(undoAction);
	toolBar->addAction(redoAction);

	toolBar->addSeparator();          // 添加分割线
	toolBar->addAction(deleteAction);

	toolBar->addSeparator();
	toolBar->addAction(moveTopAction);
	toolBar->addAction(moveBottomAction);
	toolBar->addAction(moveUpAction);
	toolBar->addAction(moveDownAction);

	toolBar->setIconSize(QSize(20, 20));
}

void MainWindow::setupDockWidgets()
{
	// 图形库
	ShapeLibraryWidget* shapeLibWidget = new ShapeLibraryWidget(this);
	QScrollArea* shapeLibScroll = new QScrollArea(this);                       // 创建滚动区域
	shapeLibScroll->setWidget(shapeLibWidget);                                 // 将图形库窗口添加至滚动区域中
	shapeLibScroll->setWidgetResizable(true);                                  // 设置滚动区域的内容控件自动调整大小以适应视口变化
	shapeLibScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	shapeLibScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QDockWidget* leftDock = new QDockWidget(this);          // 创建左侧停靠窗口
	leftDock->setTitleBarWidget(new QWidget());             // 隐藏标题栏
	leftDock->setWidget(shapeLibScroll);
	leftDock->setFixedWidth(270);                           // 设置固定宽度
	leftDock->setMaximumWidth(400);                         // 设置最大宽度
	leftDock->setFeatures(QDockWidget::DockWidgetMovable);  // 设置窗口可移动
	addDockWidget(Qt::LeftDockWidgetArea, leftDock);

	// 属性面板
	propertyPanel = new PropertyPanel(this);
	QDockWidget* rightDock = new QDockWidget(this);         // 创建右侧停靠窗口
	rightDock->setTitleBarWidget(new QWidget());            // 隐藏标题栏
	rightDock->setWidget(propertyPanel);
	rightDock->setFixedWidth(280);
	rightDock->setFeatures(QDockWidget::NoDockWidgetFeatures);  // 禁用停靠窗口功能
	addDockWidget(Qt::RightDockWidgetArea, rightDock);
}

void MainWindow::updateActions()
{
	undoAction->setEnabled(drawArea->canUndo());
	redoAction->setEnabled(drawArea->canRedo());
	copyAction->setEnabled(drawArea->canCopy());
	pasteAction->setEnabled(drawArea->canPaste());
	cutAction->setEnabled(drawArea->canCut());
	deleteAction->setEnabled(drawArea->canDelete());
	moveTopAction->setEnabled(drawArea->canMoveTop());
	moveBottomAction->setEnabled(drawArea->canMoveBottom());
	moveUpAction->setEnabled(drawArea->canMoveUp());
	moveDownAction->setEnabled(drawArea->canMoveDown());
}