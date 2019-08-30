    #include "CustomWindow.h"
    #include "ui_CustomWindow.h"
    #include <QPainter>
    #include <QMouseEvent>
    #include <QDesktopWidget>
    #include <QCursor>
    #include <QSettings>
    #include <QStyleOption>
    #include <QMenu>
    #include <QTextOption>

    #include <QTextCodec>
    #include <QFile>
    #include <QFontDatabase>
    #include <QTextStream>
    #include <QMessageBox>
    #include <QSet>
    #include <QInputDialog>
    #include <chrono>
    #include "edit_dist.h"
    #include "meters-def.h"

    #define TOTAL_DICT_WORDS 99217

    CustomWindow::CustomWindow(QWidget *parent) : QWidget(parent), ui(new Ui::CustomWindow)
    {
        QFile file(":/CustomWindow.css");
        QFontDatabase::addApplicationFont(":/new/fonts/Mehr Nastaliq Web version 1.0 beta (1).ttf");
        QFontDatabase::addApplicationFont(":/new/fonts/Apalu.ttf");
        QFontDatabase::addApplicationFont(":/new/fonts/assistant.regular.ttf");
        if (file.open(QFile::ReadOnly))
        {
            setStyleSheet(file.readAll());
            file.close();
        }

        ui->setupUi(this);

        setWindowFlags(Qt::CustomizeWindowHint);
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_DeleteOnClose);
        setMouseTracking(true);

        ui->titleBar->setMouseTracking(true);
        ui->LTitle->setMouseTracking(true);
        ui->tbMenu->setMouseTracking(true);
        ui->pbMin->setMouseTracking(true);
        ui->pbMax->setMouseTracking(true);
        ui->pbClose->setMouseTracking(true);
        ui->centralWidget->setMouseTracking(true);
        //   ui->textEdit->setAlignment(Qt::AlignRight);
        addAction(ui->actionClose);

        connect(ui->pbMin, &QPushButton::clicked, this, &CustomWindow::minimizeBtnClicked);
        connect(ui->pbMax, &QPushButton::clicked, this, &CustomWindow::maximizeBtnClicked);
        connect(ui->pbClose, &QPushButton::clicked, this, &CustomWindow::close);

        QSettings settings("dialogs.ini", QSettings::IniFormat);
        const auto geo = settings.value("geometry").toRect();
        const auto desktopSize = QApplication::desktop();

        if (geo.height() > 0 && geo.x() < desktopSize->width() && geo.width() > 0 && geo.y() < desktopSize->height())
            setGeometry(geo);

        if (settings.value("maximized").toBool())
        {
            showMaximized();
            ui->pbMax->setIcon(QIcon(":/ui/images/app_rest.png"));
        }
        push_button_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
        push_button_2_shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);

        connect(this->push_button_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_clicked()));
        connect(this->push_button_2_shortcut, SIGNAL(activated()), this, SLOT(on_pushButton_2_clicked()));


        islah_but_stylesheet = taqti_but_stylesheet;

        ui->taqtiButton->setStyleSheet("background-color: #cfd8dc;");

        textedit_html = prev_islah_text = prev_taqti_text = "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'><html><head><meta name='qrichtext' content='1' /><style type='text/css'>* { align:right; } p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Assistant, Mehr Nastaliq Web'; font-size:15pt; font-weight:400; font-style:normal;\"><p align='right' style='-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'></p></body></html>";

        mode = ProgramMode::TAQTI;

        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

        QString dict_file_path = "data/words_murrab_weight_unique.txt";

        QFile file_read(dict_file_path);

        if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Error", "Unable to open dictionary to fetch words");
            close();
        }

        QTextStream text_stream(&file_read);

        dict_cache.reserve(TOTAL_DICT_WORDS * 2);

        while(!text_stream.atEnd())
        {
            QStringList line = text_stream.readLine().split(',');

            if (line.size() != 3) continue;

            dict_cache.insertMulti(line[0], {line[0], line[1], line[2]}); // With word as a key
        }

        file_read.close();
    }

    CustomWindow::~CustomWindow()
    {
        QSettings settings("dialogs.ini", QSettings::IniFormat);
        settings.setValue("geometry", geometry());
        settings.setValue("maximized", isMaximized());

        delete ui;
    }

    void CustomWindow::mouseMoveEvent(QMouseEvent *e)
    {
        const auto xMouse = e->pos().x();
        const auto yMouse = e->pos().y();
        const auto wWidth = geometry().width();
        const auto wHeight = geometry().height();

        if (mMoveWidget)
        {
            mInResizeZone = false;
            moveWindow(e);
        }
        else if (mAllowResize)
            resizeWindow(e);
        else if (xMouse >= wWidth - PIXELS_TO_ACT) //Cursor right side
        {
            mInResizeZone = true;

            if (yMouse >= wHeight - PIXELS_TO_ACT)
                setCursor(Qt::SizeFDiagCursor);
            else if (yMouse <= PIXELS_TO_ACT)
                setCursor(Qt::SizeBDiagCursor);
            else
                setCursor(Qt::SizeHorCursor);

            resizeWindow(e);
        }
        else if (xMouse <= PIXELS_TO_ACT) //Cursor left side
        {
            mInResizeZone = true;

            if (yMouse >= wHeight - PIXELS_TO_ACT)
                setCursor(Qt::SizeBDiagCursor);
            else if (yMouse <= PIXELS_TO_ACT)
                setCursor(Qt::SizeFDiagCursor);
            else
                setCursor(Qt::SizeHorCursor);

            resizeWindow(e);
        }
        else if (yMouse >= wHeight - PIXELS_TO_ACT) //Cursor low side
        {
            mInResizeZone = true;
            setCursor(Qt::SizeVerCursor);

            resizeWindow(e);
        }
        else if (yMouse <= PIXELS_TO_ACT) //Cursor top side
        {
            mInResizeZone = true;
            setCursor(Qt::SizeVerCursor);

            resizeWindow(e);
        }
        else
        {
            mInResizeZone = false;
            setCursor(Qt::ArrowCursor);
        }

        e->accept();
    }

    void CustomWindow::mousePressEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            const auto xPos = e->pos().x();
            const auto yPos = e->pos().y();

            if (mInResizeZone)
            {
                mAllowResize = true;

                if (yPos <= PIXELS_TO_ACT)
                {
                    if (xPos <= PIXELS_TO_ACT)
                        mResizeDiagTopLeft = true;
                    else if (xPos >= geometry().width() - PIXELS_TO_ACT)
                        mResizeDiagTopRight = true;
                    else
                        mResizeVerTop = true;
                }
                else if (xPos <= PIXELS_TO_ACT)
                    mResizeHorLeft = true;
            }
            else if (xPos >= PIXELS_TO_ACT && xPos < ui->titleBar->geometry().width()
                     && yPos >= PIXELS_TO_ACT && yPos < ui->titleBar->geometry().height())
            {
                mMoveWidget = true;
                mDragPosition = e->globalPos() - frameGeometry().topLeft();
            }
        }

        e->accept();
    }

    void CustomWindow::mouseReleaseEvent(QMouseEvent *e)
    {
        mMoveWidget = false;
        mAllowResize = false;
        mResizeVerTop = false;
        mResizeHorLeft = false;
        mResizeDiagTopLeft = false;
        mResizeDiagTopRight = false;

        e->accept();
    }

    void CustomWindow::mouseDoubleClickEvent(QMouseEvent *e)
    {
        const auto tbMenuGeo = ui->tbMenu->geometry();
        const auto titleBarGeo = ui->titleBar->geometry();
        const auto xPos = e->pos().x();
        const auto yPos = e->pos().y();

        if (xPos < tbMenuGeo.right() && yPos < tbMenuGeo.bottom() && xPos >= tbMenuGeo.x() && yPos >= tbMenuGeo.y() && ui->tbMenu->isVisible())
            close();
        else if (mTitleMode != TitleMode::FullScreenMode && xPos < titleBarGeo.width() && yPos < titleBarGeo.height())
            maximizeBtnClicked();

        e->accept();
    }

    void CustomWindow::paintEvent (QPaintEvent *)
    {
        QStyleOption opt;
        opt.init (this);

        QPainter p(this);
        style()->drawPrimitive (QStyle::PE_Widget, &opt, &p, this);
    }

    void CustomWindow::moveWindow(QMouseEvent *e)
    {
        if (e->buttons() & Qt::LeftButton)
        {
            move(e->globalPos() - mDragPosition);
            e->accept();
        }
    }

    void CustomWindow::resizeWindow(QMouseEvent *e)
    {
        if (mAllowResize)
        {
            const auto cursorShape = cursor().shape();

            switch (cursorShape)
            {
            case Qt::SizeVerCursor:
                resizeWhenVerCursor(e->pos());
                break;
            case Qt::SizeHorCursor:
                resizeWhenHorCursor(e->pos());
                break;
            case Qt::SizeBDiagCursor:
                resizeWhenBDiagCursor(e->pos());
                break;
            case Qt::SizeFDiagCursor:
                resizeWhenFDiagCursor(e->pos());
                break;
            default: break;
            }

            e->accept();
        }
    }

    void CustomWindow::resizeWhenVerCursor (const QPoint &p)
    {
        auto yMouse = p.y();
        auto wWidth = geometry().width();
        auto wHeight = geometry().height();

        if (mResizeVerTop)
        {
            int newY = geometry().y() + yMouse;
            int newHeight = wHeight - yMouse;

            if (newHeight > minimumSizeHint().height())
            {
                resize(wWidth, newHeight);
                move(geometry().x(), newY);
            }
        }
        else
            resize(wWidth, yMouse+1);
    }

    void CustomWindow::resizeWhenHorCursor (const QPoint &p)
    {
        auto xMouse = p.x();
        auto wWidth = geometry().width();
        auto wHeight = geometry().height();

        if (mResizeHorLeft)
        {
            int newX = geometry().x() + xMouse;
            int newWidth = wWidth - xMouse;

            if (newWidth > minimumSizeHint().width())
            {
                resize(newWidth, wHeight);
                move(newX, geometry().y());
            }
        }
        else
            resize(xMouse, wHeight);
    }

    void CustomWindow::resizeWhenBDiagCursor(const QPoint &p)
    {
        auto xMouse = p.x();
        auto yMouse = p.y();
        auto wWidth = geometry().width();
        auto wHeight = geometry().height();
        auto newX = 0;
        auto newWidth = 0;
        auto newY = 0;
        auto newHeight = 0;

        if (mResizeDiagTopRight)
        {
            newX = geometry().x();
            newWidth = xMouse;
            newY = geometry().y() + yMouse;
            newHeight = wHeight - yMouse;
        }
        else
        {
            newX = geometry().x() + xMouse;
            newWidth = wWidth - xMouse;
            newY = geometry().y();
            newHeight = yMouse;
        }

        if (newWidth >= minimumSizeHint().width() && newHeight >= minimumSizeHint().height())
        {
            resize(newWidth, newHeight);
            move(newX, newY);
        }
        else if (newWidth >= minimumSizeHint().width())
        {
            resize(newWidth, wHeight);
            move(newX, geometry().y());
        }
        else if (newHeight >= minimumSizeHint().height())
        {
            resize(wWidth, newHeight);
            move(geometry().x(), newY);
        }
    }

    void CustomWindow::resizeWhenFDiagCursor(const QPoint &p)
    {
        auto xMouse = p.x();
        auto yMouse = p.y();
        auto geoX = geometry().x();
        auto geoY = geometry().y();
        auto wWidth = geometry().width();
        auto wHeight = geometry().height();

        if (mResizeDiagTopLeft)
        {
            int newX = geoX + xMouse;
            int newWidth = wWidth - xMouse;
            int newY = geoY + yMouse;
            int newHeight = wHeight - yMouse;

            if (newWidth >= minimumSizeHint().width() && newHeight >= minimumSizeHint().height())
            {
                resize(newWidth, newHeight);
                move(newX, newY);
            }
            else if (newWidth >= minimumSizeHint().width())
            {
                resize(newWidth, wHeight);
                move(newX, geoY);
            }
            else if (newHeight >= minimumSizeHint().height())
            {
                resize(wWidth, newHeight);
                move(geoX, newY);
            }
        }
        else
            resize(xMouse+1, yMouse+1);
    }

    void CustomWindow::setCentralWidget(QWidget *widget, const QString &widgetName)
    {
        //We do connect in the old fashion way since it will be decided in execution time
        connect(widget, SIGNAL(cancelled()), this, SLOT(close()));

        ui->verticalLayout->addWidget(widget);
        ui->LTitle->setText(widgetName);
    }

    void CustomWindow::setTitlebarMode(const TitleMode &flag)
    {
        mTitleMode = flag;

        switch (mTitleMode)
        {
        case TitleMode::CleanTitle:
            ui->tbMenu->setHidden(true);
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            ui->pbClose->setHidden(true);
            break;
        case TitleMode::OnlyCloseButton:
            ui->tbMenu->setHidden(true);
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            break;
        case TitleMode::MenuOff:
            ui->tbMenu->setHidden(true);
            break;
        case TitleMode::MaxMinOff:
            ui->pbMin->setHidden(true);
            ui->pbMax->setHidden(true);
            break;
        case TitleMode::FullScreenMode:
            ui->pbMax->setHidden(true);
            showMaximized();
            break;
        case TitleMode::MaximizeModeOff:
            ui->pbMax->setHidden(true);
            break;
        case TitleMode::MinimizeModeOff:
            ui->pbMin->setHidden(true);
            break;
        case TitleMode::FullTitle:
            ui->tbMenu->setVisible(true);
            ui->pbMin->setVisible(true);
            ui->pbMax->setVisible(true);
            ui->pbClose->setVisible(true);
            break;
        }
        ui->LTitle->setVisible(true);
    }

    void CustomWindow::setTitlebarMenu(QMenu *menu, const QString &icon)
    {
        ui->tbMenu->setMenu(menu);
        ui->tbMenu->setIcon(QIcon(icon));
    }

    void CustomWindow::maximizeBtnClicked()
    {
        if (isFullScreen() || isMaximized())
        {
            ui->pbMax->setIcon(QIcon(":/ui/images/app_max.png"));
            setWindowState(windowState() & ~Qt::WindowFullScreen & ~Qt::WindowMaximized);
        }
        else
        {
            ui->pbMax->setIcon(QIcon(":/ui/images/app_rest.png"));
            setWindowState(windowState() | Qt::WindowFullScreen | Qt::WindowMaximized);
        }
    }

    void CustomWindow::minimizeBtnClicked()
    {
        if (isMinimized())
            setWindowState(windowState() & ~Qt::WindowMinimized);
        else
            setWindowState(windowState() | Qt::WindowMinimized);
    }

    QString CustomWindow::remove_symbols(const QString& user_entered_word)
    {
        QString new_word;

        for (int i = 0; i < user_entered_word.size(); i++)
        {

            if (((user_entered_word[i] >= 1613 && user_entered_word[i] <= 1618) || (user_entered_word[i] == 1556)) && (user_entered_word[i] != 1616 || i != user_entered_word.size() - 1) )
                continue;

            new_word += user_entered_word[i];
        }

        return new_word;

    }

    QVector<QStringList> CustomWindow::get_user_input()
    {

        auto start = std::chrono::high_resolution_clock::now();

        QVector<QStringList> user_input;

        QString user_entered_combined_words = ui->textEdit->toPlainText();

        QStringList user_entered_individual_lines = user_entered_combined_words.split(QRegExp("\\s*\n"), QString::SkipEmptyParts);

        int total_lines = user_entered_individual_lines.size();

        user_input.resize(total_lines);

        for(int i = 0; i < total_lines; i++)
        {
            QStringList individual_words = user_entered_individual_lines[i].split(QRegExp(u8"[\\s+،!؟؛.۔\"]"), QString::SkipEmptyParts);

            for (int j = 0; j < individual_words.size(); j++)
            {
                individual_words[j] = remove_symbols(individual_words[j]);
            }

            user_input[i] = individual_words;
        }

        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Splitting user input: " << end.count() << "\n";

        return user_input;
    }

    QVector<QStringList> CustomWindow::get_murrab_weight(const QStringList& user_entered_line)
    {
        auto start = std::chrono::high_resolution_clock::now();
        QVector<QStringList> words_murrabs_weights(user_entered_line.size());


        for (int i = 0; i < user_entered_line.size(); i++)
        {

            QString word = user_entered_line[i];
            QString last_two_letters = word;
            QString last_three_letters = word;

            if (word.size() > 1)
            {
                last_two_letters = word.mid(word.size() - 2, 2);
            }
            if (word.size() > 2)
            {
                last_three_letters = word.mid(word.size() - 3, 3);
            }

            bool found_hamza_e_izafat  = (word.back() == L'ۂ' || last_two_letters == u8"ۂ" );

            if (found_hamza_e_izafat)
            {
                word.chop( (word.back() == L'ۂ') ? 1:2 );
                word += u8"ہِ";
            }

            // Chop (remove) the zer if we found it at the end of the word
            bool found_zaer = ( word.back() == L'ِ' );

            if (found_zaer)
            {
                word.chop(1);

                if (word.size() > 1)
                {
                    last_two_letters = word.mid(word.size() - 2, 2);
                }
                if (word.size() > 2)
                {
                    last_three_letters = word.mid(word.size() - 3, 3);
                }
            }

            bool found_ea = (word.size() > 3 && last_two_letters == u8"ئے" && word != u8"ہوئے" );

            //      if (found_ea)
            //        {
            //          word = word.chopped(2);
            //        }

            bool found_noon_ghunna = (word.back() == L'ں');

            bool found_oun = (last_two_letters == u8"وں");

            bool found_oun_yen = (last_two_letters == u8"یں" || last_two_letters == u8"وں");

            bool found_bariye = (word.back() == L'ے' && last_two_letters != u8"ئے");

            bool found_wao = (word.back() == L'و');

            bool found_an = (word.size() > 3 && last_two_letters == u8"ان");

            bool found_aan = (word.size() > 3 && last_two_letters == u8"اں");

            bool found_oun_hamzawao = (word.size() > 2 && last_two_letters == u8"ؤں");

            bool found_gan = (word.size() > 4 && last_three_letters == u8"گان");

            bool found_gaan = (word.size() > 4 && last_three_letters == u8"گاں");

            bool found_hamza_yen = (word.size() > 4 && last_three_letters == u8"ئیں");

            QChar first_letter = word.front(); // Checking the first letter of current word

            auto AllowedFirstLetter_find_iterator =  AllowedFirstLetter_set.find(first_letter.unicode()); // Find first character of user entered word in our letter map and its starting position in dictionary

            if (AllowedFirstLetter_find_iterator == AllowedFirstLetter_set.end()) // We did't find the letter in AllowedFirstLetter_set
            {
                continue;
            }

            auto dict_cache_find_iterator = dict_cache.find(word);

            if (dict_cache_find_iterator != dict_cache.end())
            {
                words_murrabs_weights[i] = dict_cache_find_iterator.value();

                if (found_zaer || found_ea || found_hamza_e_izafat || found_oun_yen || found_bariye || found_noon_ghunna)
                {
                    words_murrabs_weights[i][0] = user_entered_line[i];
                }

            }

            if (found_ea && dict_cache_find_iterator == dict_cache.end())
            {

                word.chop(2);

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][0] += u8"ئے";
                }

            }

            else if (found_noon_ghunna && dict_cache_find_iterator == dict_cache.end()) // found non ghunna at the end of the word
            {
                word.back() = L'ن';

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][0] = user_entered_line[i];
                    words_murrabs_weights[i][2].chop(1);
                }

                else if (found_oun_yen) // we didn't find word after replacing nun-ghuna with nun so, seeing if the word possibly end on oun and yen and checking for word in dict after chopping these letters from user entered word
                {
                    word.chop(2);

                    dict_cache_find_iterator = dict_cache.find(word);

                    if (dict_cache_find_iterator != dict_cache.end())
                    {
                        words_murrabs_weights[i] = dict_cache_find_iterator.value();
                        words_murrabs_weights[i][0] = user_entered_line[i];
                        word+= u8"وں";
                    }

                    else if (found_oun) // we were unable to find the word even after chopping the oun and yun, if the word only had oun at its end we would like to add 'he' at its place and then check for word again (here the word will be already chopped so we just need to append 'he')
                    {
                        word += u8"ہ";

                        dict_cache_find_iterator = dict_cache.find(word);

                        if (dict_cache_find_iterator != dict_cache.end())
                        {
                            words_murrabs_weights[i] = dict_cache_find_iterator.value();
                            words_murrabs_weights[i][0] = user_entered_line[i];
                            word+=u8"وں";
                        }

                    }

                    else if (found_hamza_yen)
                    {
                        word.chop(1); // We already chopped two letters for normal 'yen' check

                        dict_cache_find_iterator = dict_cache.find(word);

                        if (dict_cache_find_iterator != dict_cache.end())
                        {
                            words_murrabs_weights[i] = dict_cache_find_iterator.value();
                            words_murrabs_weights[i][0] = user_entered_line[i];
                        }
                    }
                }

                else if (found_aan)
                {
                    word.chop(2);

                    dict_cache_find_iterator = dict_cache.find(word);

                    if (dict_cache_find_iterator != dict_cache.end())
                    {
                        words_murrabs_weights[i] = dict_cache_find_iterator.value();

                        words_murrabs_weights[i][2].back() = L'1';

                        words_murrabs_weights[i][2] += L'0';

                        words_murrabs_weights[i][0] = user_entered_line[i];
                    }

                    else if (found_gaan)
                    {
                        word.back() = L'ہ';

                        dict_cache_find_iterator = dict_cache.find(word);

                        if (dict_cache_find_iterator != dict_cache.end())
                        {
                            words_murrabs_weights[i] = dict_cache_find_iterator.value();

                            words_murrabs_weights[i][2].chop(1);

                            words_murrabs_weights[i][2].back() = L'1';

                            words_murrabs_weights[i][2] += u8"10";

                            words_murrabs_weights[i][0] = user_entered_line[i];
                        }
                    }
                }

                else if (found_oun_hamzawao && dict_cache_find_iterator==dict_cache.end())
                {
                    word.chop(2);

                    dict_cache_find_iterator = dict_cache.find(word);

                    if (dict_cache_find_iterator != dict_cache.end())
                    {
                        words_murrabs_weights[i] = dict_cache_find_iterator.value();

                        words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }

            }

            else if (found_bariye && dict_cache_find_iterator==dict_cache.end())
            {

                word.back() = L'ا';

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][0] = user_entered_line[i];

                }

                else
                {
                    word.back() = L'ہ';

                    dict_cache_find_iterator = dict_cache.find(word);

                    if (dict_cache_find_iterator != dict_cache.end())
                    {
                        words_murrabs_weights[i] = dict_cache_find_iterator.value();
                        words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }
            }

            else if (word.back()==L'ا' && last_two_letters != u8"یا" && dict_cache_find_iterator==dict_cache.end())
            {

                word = word.chopped(1);
                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][2].back()= L'1';
                    words_murrabs_weights[i][2] += L'0';
                    words_murrabs_weights[i][0] = user_entered_line[i];

                }


            }
            else if (word.back()==L'ی' && last_two_letters != u8"گی" && dict_cache_find_iterator==dict_cache.end())
            {

                word = word.chopped(1);
                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][2].back()= L'1';
                    words_murrabs_weights[i][2] += L'0';
                    words_murrabs_weights[i][0] = user_entered_line[i];

                }

            }
            else if (last_two_letters ==u8"یا" && dict_cache_find_iterator==dict_cache.end())
            {

                word = word.chopped(2);

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][2] += u8"10";
                    words_murrabs_weights[i][0] = user_entered_line[i];
                }

            }
            else if (last_two_letters ==u8"گی" && dict_cache_find_iterator==dict_cache.end())
            {

                word = word.chopped(2);
                word+= u8"ہ";
                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();
                    words_murrabs_weights[i][2].chop(1);
                    words_murrabs_weights[i][2].back()= L'1';
                    words_murrabs_weights[i][2] += u8"10";
                    words_murrabs_weights[i][0] = user_entered_line[i];
                }

            }

            else if (found_wao && dict_cache_find_iterator==dict_cache.end())
            {
                word.chop(1);

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();


                    words_murrabs_weights[i][2].back() = L'1';

                    words_murrabs_weights[i][2] += L'0';

                    words_murrabs_weights[i][0] = user_entered_line[i];
                }
            }

            else if (found_an && dict_cache_find_iterator==dict_cache.end())
            {
                word.chop(2);

                dict_cache_find_iterator = dict_cache.find(word);

                if (dict_cache_find_iterator != dict_cache.end())
                {
                    words_murrabs_weights[i] = dict_cache_find_iterator.value();

                    words_murrabs_weights[i][2].back() = L'1';

                    words_murrabs_weights[i][2] += u8"01";

                    words_murrabs_weights[i][0] = user_entered_line[i];
                }

                else if (found_gan && dict_cache_find_iterator==dict_cache.end())
                {
                    word.back() = L'ہ';

                    dict_cache_find_iterator = dict_cache.find(word);

                    if (dict_cache_find_iterator != dict_cache.end())
                    {
                        words_murrabs_weights[i] = dict_cache_find_iterator.value();

                        words_murrabs_weights[i][2].chop(1);

                        words_murrabs_weights[i][2].back() = L'1';

                        words_murrabs_weights[i][2] += u8"101";

                        words_murrabs_weights[i][0] = user_entered_line[i];
                    }
                }

            }
        }

        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Fetching Weights: " << end.count() << "\n";

        return words_murrabs_weights;
    }

    bool CustomWindow::has_different_weights(QString word)
    {

        if (word.isEmpty())
        {
            return false;
        }

        else if (word.back() == 1616) // word has 'zaer' at the end
        {
            word.chop(1);
        }

        QSet<QString> different_unique_weights;

        auto dict_cache_find_iterator = dict_cache.find(word);

        while (dict_cache_find_iterator != dict_cache.end() && dict_cache_find_iterator.key() == word)
        {
            if (dict_cache_find_iterator.value().size() < 3)
            {
                dict_cache_find_iterator++;
                continue;
            }

            different_unique_weights.insert(dict_cache_find_iterator.value()[2]);

            dict_cache_find_iterator++;

            if (different_unique_weights.size() > 1)
                return true;
        }

        return different_unique_weights.size() > 1;
    }

    QList<QString> CustomWindow::get_different_weights_of_word(QString word)
    {
        if (word.isEmpty())
        {
            return QList<QString>();
        }

        else if (word.back() == 1616)
        {
            word.chop(1);
        }

        QSet<QString> different_unique_weights;

        auto dict_cache_find_iterator = dict_cache.find(word);

        while(dict_cache_find_iterator != dict_cache.end() && dict_cache_find_iterator.key() == word)
        {
            if (dict_cache_find_iterator.value().size() < 3)
            {
                dict_cache_find_iterator++;
                continue;
            }

            QString weight = dict_cache_find_iterator.value()[2];

            different_unique_weights.insert(weight);

            dict_cache_find_iterator++;
        }

        return different_unique_weights.toList();
    }

    QString accumulate(const QStringList& weights, QString seperator = "")
    {
        QString accum;

        for (int i = 0; i < weights.size(); i++)
        {
            accum += weights[i] + ( i == weights.size() - 1 ? "" : seperator);
        }

        return accum;
    }

    Accumulated_Weight CustomWindow::islah(QVector<Accumulated_Weight> accumulated_weights_per_line, const QVector<QString>& meters)
    {

        QVector<Accumulated_Weight> orig_accumulated_weight_per_line = accumulated_weights_per_line;

        Accumulated_Weight best_accumulated_weight;
        int best_rejected_count = INT_MAX;

        for (int h = 0; h < meters.size(); h++)
        {
            std::wstring meter_bin = meters[h].toStdWString();

            Accumulated_Weight min_rejected_accumulated_weight;
            int min_rejected_count = INT_MAX;

            accumulated_weights_per_line = orig_accumulated_weight_per_line;

            for (int i = 0; i < accumulated_weights_per_line.size(); i++)
            {

                Q_ASSERT(accumulated_weights_per_line[i].bin == accumulate(accumulated_weights_per_line[i].weights));
                Q_ASSERT(accumulated_weights_per_line[i].weights.size() == accumulated_weights_per_line[i].words.size());
                Q_ASSERT(accumulated_weights_per_line[i].words.size() == accumulated_weights_per_line[i].rejected.size());

                size_t cur_loc = 0;

                for (int j = 0; j < accumulated_weights_per_line[i].weights.size(); j++)
                {

                    std::wstring cur_weight = accumulated_weights_per_line[i].weights[j].toStdWString();

                    size_t find_loc = meter_bin.find(cur_weight, cur_loc);

                    if (find_loc != cur_loc)
                    {
                        accumulated_weights_per_line[i].rejected[j] = true;
                        accumulated_weights_per_line[i].rejected_count++;
                    }

                    cur_loc += cur_weight.size();

                }

                //  Q_ASSERT(accumulated_weights_per_line[i].rejected_count <= accumulated_weights_per_line[i].rejected.size());

                if (!accumulated_weights_per_line[i].rejected.empty() && meter_bin.size() > cur_loc)
                {
                    accumulated_weights_per_line[i].rejected.back() = true;
                    accumulated_weights_per_line[i].rejected_count++;
                }

                if (accumulated_weights_per_line[i].rejected_count != 0)
                {
                    accumulated_weights_per_line[i].has_meter = false;
                }

                if (accumulated_weights_per_line[i].rejected_count < min_rejected_count)
                {
                    min_rejected_accumulated_weight = accumulated_weights_per_line[i];
                    min_rejected_count = accumulated_weights_per_line[i].rejected_count;
                }
            }

            if (min_rejected_count < best_rejected_count)
            {
                best_rejected_count = min_rejected_count;
                best_accumulated_weight = min_rejected_accumulated_weight;
                islah_best_meter_bin = QString::fromStdWString(meter_bin);
            }
        }

        return best_accumulated_weight;
    }

    // Returns the index of word weight in accumulated weights that has first letter letter on given index
    int has_first_letter_on_index (const Accumulated_Weight& accumulated_weight, int index)
    {
        Q_ASSERT(accumulated_weight.bin == accumulate(accumulated_weight.weights));

        if (index < 0 || index >= accumulated_weight.bin.size() || accumulated_weight.weights.isEmpty()) return -1;

        int cur_index = 0;

        for (int i = 0; i < accumulated_weight.weights.size(); i++)
        {
            if (cur_index == index && accumulated_weight.bin[cur_index] == L'1') return i;
            else if (cur_index > index) return -1;

            cur_index += accumulated_weight.weights[i].size();
        }

        return -1;
    }


    QStringList get_matched_meters(QVector<Accumulated_Weight>& accumulated_weights)
    {
        QStringList meters;

        for (int i = 0; i < accumulated_weights.size(); i++)
        {

            if(!accumulated_weights[i].bin.isEmpty() && accumulated_weights[i].bin.back()=='1')
            {
                accumulated_weights[i].bin.chop(1);
                accumulated_weights[i].weights.back().chop(1);
                accumulated_weights[i].is_tasbeegh_o_azala = true;
            }

            QString accumlated_weights = accumulate(accumulated_weights[i].weights);

            auto meter_find_iterator = Meter_map.find(accumulated_weights[i].bin.toStdWString());

            if (meter_find_iterator != Meter_map.end())
            {
                if ( accumulated_weights[i].bin == u8"1011011010101101101101010" || accumulated_weights[i].bin == u8"10110110101011011011010101" ||
                     accumulated_weights[i].bin == u8"1010110110101101011011010" || accumulated_weights[i].bin == u8"10101101101011010110110101"
                     || accumulated_weights[i].bin == u8"1110110110101111011011010" || accumulated_weights[i].bin == u8"11101101101011110110110101"
                     || accumulated_weights[i].bin == u8"1010111010101101011101010" || accumulated_weights[i].bin == u8"10101110101011010110110101")
                {
                    int index = has_first_letter_on_index(accumulated_weights[i], 12);
                    if (index != -1)
                    {
                        accumulated_weights[i].rejected[index] = true;
                        continue;
                    }
                }

                meters.push_back(QString::fromStdWString(meter_find_iterator->first));

                if(accumulated_weights.size() >= 2)
                {
                    if(accumulated_weights[i].bin.back() == L'1')
                        accumulated_weights[i].is_tasbeegh_o_azala = true;
                }

            };
        }

        return meters;
    }

    QVector<Accumulated_Weight> CustomWindow::get_accumulated_weight(const QVector<QStringList>& words_murrab_weight_per_line)
    {
        auto start = std::chrono::high_resolution_clock::now();

        int size = words_murrab_weight_per_line.size();

        if(size <= 0)
            return QVector<Accumulated_Weight>();

        QVector<Accumulated_Weight> accumulated_weights(1, {}); // There will always be one accumulated weight

        QChar prev_word_last_letter;

        int new_accumulated_weight_size = accumulated_weights.size();
        int prev_accumulated_weight_size = new_accumulated_weight_size;

        bool has_word_not_present_in_dict = false;

        for (int i = 0; i < size; i++) // Iterating every word
        {

            if (words_murrab_weight_per_line[i].size() != 3)
            {
                has_word_not_present_in_dict = true;
                continue;
            }

            QString individual_word = words_murrab_weight_per_line[i][0];
            QString last_two_letters = individual_word;
            QString last_three_letters = individual_word;

            if (individual_word.size() > 1)
            {
                last_two_letters = individual_word.mid(individual_word.size() - 2, 2);
            }
            if (individual_word.size() > 2)
            {
                last_three_letters = individual_word.mid(individual_word.size() - 3, 3);
            }

            QString individual_weight = words_murrab_weight_per_line[i][2];

            QChar last_letter = individual_word.back();
            QChar first_letter = individual_word.front();
            QChar last_weight = individual_weight.back();

            bool has_multiple_weights = has_different_weights(individual_word);

            if (!has_multiple_weights)
            {
                for (int j = 0; j < accumulated_weights.size(); j++)
                {
                    if (individual_word == u8"و")
                        break;

                    accumulated_weights[j].bin += individual_weight;
                    accumulated_weights[j].words.push_back(individual_word);
                    accumulated_weights[j].weights.push_back(individual_weight);
                    accumulated_weights[j].rejected.push_back(false);

                    Q_ASSERT(accumulated_weights[j].bin == accumulate(accumulated_weights[j].weights));
                }
            }

            else
            {
                QList<QString> different_weights = get_different_weights_of_word(individual_word);

                accumulated_weights.reserve(accumulated_weights.size() * different_weights.size());

                if (i == 0)
                {
                    accumulated_weights.pop_back();
                    new_accumulated_weight_size = prev_accumulated_weight_size = 0;
                }

                for (QString weight: different_weights)
                {
                    if (i == 0)
                    {
                        Accumulated_Weight new_acc_weight;
                        new_acc_weight.bin = weight;
                        new_acc_weight.words.push_back(individual_word);
                        new_acc_weight.weights.push_back(weight);
                        new_acc_weight.rejected.push_back(false);

                        accumulated_weights.push_back(new_acc_weight);

                        Q_ASSERT(new_acc_weight.bin == accumulate(new_acc_weight.weights));

                        new_accumulated_weight_size++;
                    }
                    else
                    {
                        for (int k = 0; k < prev_accumulated_weight_size; k++)
                        {
                            Accumulated_Weight new_acc_weight;
                            new_acc_weight.bin = accumulated_weights[k].bin + weight;
                            new_acc_weight.weights = accumulated_weights[k].weights;
                            new_acc_weight.words = accumulated_weights[k].words;
                            new_acc_weight.rejected = accumulated_weights[k].rejected;

                            new_acc_weight.weights.push_back(weight);
                            new_acc_weight.words.push_back(individual_word);
                            new_acc_weight.rejected.push_back(false);

                            accumulated_weights.push_back(new_acc_weight);


                            Q_ASSERT(new_acc_weight.bin == accumulate(new_acc_weight.weights));

                            new_accumulated_weight_size++;
                        }
                    }
                }

                for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                    accumulated_weights.pop_front();
                    new_accumulated_weight_size--;
                }

                prev_accumulated_weight_size = new_accumulated_weight_size;
            }

            if (i != 0 && (prev_word_last_letter != L'ا' && prev_word_last_letter != L'ہ' && prev_word_last_letter != L'ۂ' &&
                            prev_word_last_letter != L'ے' && prev_word_last_letter != L'ؤ' && prev_word_last_letter != L'ں'))
            {

                if (first_letter == L'ا')
                {
                    for (int k = 0; k < prev_accumulated_weight_size; k++)
                    {
                        if (accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1 >= 0)
                        {
                            Accumulated_Weight new_accumulated_weight;

                            new_accumulated_weight.bin = accumulated_weights[k].bin;

                            new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1] = L'1';
                            new_accumulated_weight.bin.remove(accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size(), 1);

                            new_accumulated_weight.weights = accumulated_weights[k].weights;
                            new_accumulated_weight.words = accumulated_weights[k].words;
                            new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                            new_accumulated_weight.weights[new_accumulated_weight.weights.size() - 2].back() = L'1';
                            new_accumulated_weight.weights.back().remove(0, 1);

                            accumulated_weights.push_back(new_accumulated_weight);

                            Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));
                            new_accumulated_weight_size++;
                        }
                    }

                }

                else if (first_letter == L'آ')
                {
                    for (int k = 0; k < prev_accumulated_weight_size; k++)
                    {

                        if (accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1 >= 0)
                        {
                            Accumulated_Weight new_accumulated_weight;

                            new_accumulated_weight.bin = accumulated_weights[k].bin;

                            new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() - 1] = L'1';
                            new_accumulated_weight.bin[accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size() + 1] = L'0';
                            new_accumulated_weight.bin.remove(accumulated_weights[k].bin.size() - accumulated_weights[k].weights.back().size(), 1);

                            new_accumulated_weight.weights = accumulated_weights[k].weights;
                            new_accumulated_weight.words = accumulated_weights[k].words;
                            new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                            new_accumulated_weight.weights[new_accumulated_weight.weights.size() - 2].back() = L'1';
                            new_accumulated_weight.weights.back().remove(0, 1);
                            new_accumulated_weight.weights.back().front() = L'0';

                            accumulated_weights.push_back(new_accumulated_weight);

                            Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                            new_accumulated_weight_size++;
                        }
                    }
                }

                prev_accumulated_weight_size = new_accumulated_weight_size;
            }


            if (individual_word.size() > 3 && last_two_letters == u8"ئے" && dict_cache.find(individual_word) == dict_cache.end())
            {
                for (int k = 0; k < prev_accumulated_weight_size; k++)
                {

                    accumulated_weights[k].bin += L'1';
                    accumulated_weights[k].weights.back() += L'1';

                    Accumulated_Weight new_accumulated_weight_one;
                    new_accumulated_weight_one.bin = accumulated_weights[k].bin + L'0';
                    new_accumulated_weight_one.weights = accumulated_weights[k].weights;
                    new_accumulated_weight_one.words = accumulated_weights[k].words;
                    new_accumulated_weight_one.rejected = accumulated_weights[k].rejected;

                    new_accumulated_weight_one.weights.back() += L'0';

                    if (last_three_letters== u8"وئے")
                    {
                        Accumulated_Weight new_accumulated_weight_two;
                        new_accumulated_weight_two.bin = accumulated_weights[k].bin;
                        new_accumulated_weight_two.weights = accumulated_weights[k].weights;
                        new_accumulated_weight_two.words = accumulated_weights[k].words;
                        new_accumulated_weight_two.rejected = accumulated_weights[k].rejected;

                        new_accumulated_weight_two.bin.chop(1);
                        new_accumulated_weight_two.bin.back() = L'1';

                        new_accumulated_weight_two.weights.back().chop(1);
                        new_accumulated_weight_two.weights.back().back() = L'1';

                        Accumulated_Weight new_accumulated_weight_three;
                        new_accumulated_weight_three.bin = new_accumulated_weight_two.bin;
                        new_accumulated_weight_three.weights = new_accumulated_weight_two.weights;
                        new_accumulated_weight_three.words = new_accumulated_weight_two.words;
                        new_accumulated_weight_three.rejected = new_accumulated_weight_two.rejected;

                        new_accumulated_weight_three.bin += L'0';
                        new_accumulated_weight_three.weights.back() += L'0';

                        accumulated_weights.push_back(new_accumulated_weight_two);
                        accumulated_weights.push_back(new_accumulated_weight_three);

                        Q_ASSERT(new_accumulated_weight_two.bin == accumulate(new_accumulated_weight_two.weights));
                        Q_ASSERT(new_accumulated_weight_three.bin == accumulate(new_accumulated_weight_three.weights));

                        new_accumulated_weight_size += 2;
                    }

                    accumulated_weights.push_back(new_accumulated_weight_one);

                    Q_ASSERT(new_accumulated_weight_one.bin == accumulate(new_accumulated_weight_one.weights));

                    new_accumulated_weight_size++;
                }
            }

            else if(last_letter == L'ِ' || last_letter == L'ۂ' || last_two_letters == u8"ۂ")
            {
                for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                    accumulated_weights[k].bin.back() = L'1';
                    accumulated_weights[k].weights.back().back() = L'1';

                    Accumulated_Weight new_accumulated_weight;

                    new_accumulated_weight.bin = accumulated_weights[k].bin + L'0';
                    new_accumulated_weight.weights = accumulated_weights[k].weights;
                    new_accumulated_weight.words = accumulated_weights[k].words;
                    new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                    new_accumulated_weight.weights.back() += L'0';

                    accumulated_weights.push_back(new_accumulated_weight);

                    Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                    new_accumulated_weight_size++;
                }
            }

            else if (individual_word.size() > 2 && last_weight != L'1' && (i != size - 1 && last_letter != L'ؤ') && (last_letter == L'ا' || last_letter == L'ہ' ||
                                                                                                                     last_letter == L'ی' || last_letter == L'ے' ||
                                                                                                                     last_letter == L'و' || last_letter == L'ؤ' )){
                for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                    Accumulated_Weight new_accumulated_weight;

                    new_accumulated_weight.bin = accumulated_weights[k].bin;
                    new_accumulated_weight.weights = accumulated_weights[k].weights;
                    new_accumulated_weight.words = accumulated_weights[k].words;
                    new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                    new_accumulated_weight.bin.chop(1);
                    new_accumulated_weight.weights.back().chop(1);

                    accumulated_weights.push_back(new_accumulated_weight);

                    Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                    new_accumulated_weight_size++;
                }

            }

            else if (individual_word == u8"و" || ((individual_word.size() > 3 && ((last_two_letters  == u8"یں" && last_three_letters != u8"ئیں")|| last_two_letters == u8"وں")) && dict_cache.find(individual_word) == dict_cache.end()))
            {
                for (int k = 0; k < prev_accumulated_weight_size; k++)
                {
                    //              if(accumulated_weights[k].back() != L'ا' && accumulated_weights[k].back() != L'و')
                    //                {
                    if(prev_word_last_letter == L'ی' && individual_word == u8"و")
                    {
                        accumulated_weights[k].bin += L'1';
                        accumulated_weights[k].weights.back() += L'1';

                        Accumulated_Weight new_accumulated_weight = accumulated_weights[k];
                        new_accumulated_weight.bin += L'0';
                        new_accumulated_weight.weights.back() += L'0';

                        Accumulated_Weight new_accumulated_weight2 = accumulated_weights[k];
                        new_accumulated_weight2.bin.chop(1);
                        new_accumulated_weight2.weights.back().chop(1);
                        new_accumulated_weight2.bin += L'1';
                        new_accumulated_weight2.weights.back() += L'1';

                        Accumulated_Weight new_accumulated_weight3 = new_accumulated_weight2;
                        new_accumulated_weight3.bin += L'0';
                        new_accumulated_weight3.weights.back() += L'0';

                        Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));
                        Q_ASSERT(new_accumulated_weight2.bin == accumulate(new_accumulated_weight2.weights));
                        Q_ASSERT(new_accumulated_weight3.bin == accumulate(new_accumulated_weight3.weights));

                        accumulated_weights.push_back(new_accumulated_weight);
                        accumulated_weights.push_back(new_accumulated_weight2);
                        accumulated_weights.push_back(new_accumulated_weight3);

                        new_accumulated_weight_size += 3;
                    }

                    else
                    {
                        accumulated_weights[k].bin.back() = L'1';
                        accumulated_weights[k].weights.back().back() = L'1';

                        Accumulated_Weight  new_accumulated_weight = accumulated_weights[k];

                        new_accumulated_weight.bin += L'0';
                        new_accumulated_weight.weights.back() += L'0';

                        Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                        accumulated_weights.push_back(new_accumulated_weight);

                        new_accumulated_weight_size++;
                    }

                }
            }

            else if (((individual_word.size() > 3 && last_two_letters == u8"ؤں")
                      || (individual_word.size() > 4 && last_three_letters == u8"ئیں"))
                     && (dict_cache.find(individual_word) == dict_cache.end()))
            {

                QString chopped_individual_word = individual_word.chopped((last_two_letters == u8"ؤں") ? 2:3 );

                if (chopped_individual_word.back() == L'ا')
                {
                    for (int k = 0; k < prev_accumulated_weight_size; k++)
                    {

                        accumulated_weights[k].bin += L'1';
                        accumulated_weights[k].weights.back() += L'1';

                        Accumulated_Weight new_accumulated_weight;
                        new_accumulated_weight.bin = accumulated_weights[k].bin + L'0';
                        new_accumulated_weight.weights = accumulated_weights[k].weights;
                        new_accumulated_weight.words = accumulated_weights[k].words;
                        new_accumulated_weight.rejected = accumulated_weights[k].rejected;

                        new_accumulated_weight.weights.back() += L'0';

                        accumulated_weights.push_back(new_accumulated_weight);

                        Q_ASSERT(new_accumulated_weight.bin == accumulate(new_accumulated_weight.weights));

                        new_accumulated_weight_size++;
                    }
                }
            }


            prev_accumulated_weight_size = new_accumulated_weight_size;
            prev_word_last_letter = individual_word.back();

        }


        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Fetching Accumulative Weight: " << end.count() << "\n";

        if (has_word_not_present_in_dict)
        {
            for (int i = 0; i < accumulated_weights.size(); i++)
            {
                accumulated_weights[i].has_meter = false;
            }
        }

        return accumulated_weights;
    }

    void CustomWindow::display_meters(const QVector<QStringList>& words_murrab_weight_per_line)
    {
        auto start = std::chrono::high_resolution_clock::now();

        int size = words_murrab_weight_per_line.size();

        //  QVector<unsigned long long> meter_vector = {};

        //  for(auto&i: Meters_in_Decimal)
        //  {
        //    meter_vector.push_back(i.first);
        //  }
        //   std::sort(meter_vector.begin(),meter_vector.end());



        //  for(auto&i: Meters_in_Decimal)
        //  {
        //    meter_vector.push_back(i.first);
        //  }
        //   std::sort(meter_vector.begin(),meter_vector.end());

        if(size <= 0)
        {
            return;
        }

        QVector<Accumulated_Weight> accumulated_weights = get_accumulated_weight(words_murrab_weight_per_line);
        bool tasbeegh_o_azala = false;
        QString add_weight ="";
        bool found_meter = false;

        ui->textEdit->insertPlainText(u8"\n\nافاعیل: ");
        int index = 0;


        for (int i = 0; i < accumulated_weights.size(); i++)
        {
            tasbeegh_o_azala = false;

            if(!accumulated_weights[i].bin.isEmpty() && accumulated_weights[i].bin.back()=='1')
            {
                accumulated_weights[i].bin.chop(1);
                tasbeegh_o_azala = true;
            }

            index = i;

            auto meters_find_iterator = Meter_map.find(accumulated_weights[i].bin.toStdWString());
            if (meters_find_iterator != Meter_map.end())
            {

                if ( accumulated_weights[i].bin == u8"1011011010101101101101010" || accumulated_weights[i].bin == u8"10110110101011011011010101" ||
                     accumulated_weights[i].bin == u8"1010110110101101011011010" || accumulated_weights[i].bin == u8"10101101101011010110110101"
                     || accumulated_weights[i].bin == u8"1110110110101111011011010" || accumulated_weights[i].bin == u8"11101101101011110110110101"
                     || accumulated_weights[i].bin == u8"1010111010101101011101010" || accumulated_weights[i].bin == u8"10101110101011010110110101")
                {
                    if (has_first_letter_on_index(accumulated_weights[i], 12) != -1)
                    {
                        continue;
                    }
                }

                QString meter_value = QString::fromStdWString(meters_find_iterator->second);
                if(accumulated_weights.size()>= 2)
                {
                    if(accumulated_weights[i].bin.back() == L'1')
                        tasbeegh_o_azala = true;
                }
                if(tasbeegh_o_azala)
                {
                    meter_value.insert(meter_value.size()-1,L'ا');
                    add_weight = "1";
                }
                ui->textEdit->insertPlainText(meter_value + " " +"(" + accumulated_weights[i].bin + add_weight + ")");
                found_meter = true;

                break;
            }
        }



        std::wstring key = L"";
        int closest_meter_index = 0;
        if (!found_meter)
        {

            unsigned int distance = 0;
            int count =0;
            for(int i=0; i<accumulated_weights.size(); ++i)
            {
                //			   auto it = std::lower_bound(Ordered_Meters.begin(), Ordered_Meters.end(), accumulated_weights[i]);
                //               auto key_address = (it - Ordered_Meters.begin());
                //               int value= distanceLevenshtein(Ordered_Meters[key_address], accumulated_weights[i]);
                //               if (value<distance || count == 0)
                //               {
                //                   distance = value;
                //                   key = Ordered_Meters[key_address].toStdWString();
                //                   ++count;
                //                   closest_meter_index = i;


                //               }

                for(auto&j: Meter_map)
                {

                    // size_t value = levenshteinSSE::levenshtein(j.first,accumulated_weights[i].toStdWString());
                    unsigned int value = levenshteinDist(j.first,accumulated_weights[i].bin.toStdWString());

                    if( value<distance || count ==0)
                    {
                        distance=value;
                        key = j.first;
                        ++count;
                        closest_meter_index = i;

                    }
                }
            }
            auto meters_find_iterator = Meter_map.find(key);
            if(meters_find_iterator!=Meter_map.end())
            {

                QString meter_value = QString::fromStdWString(meters_find_iterator->second);
                Accumulated_Weight original_weight = accumulated_weights[closest_meter_index];

                ui->textEdit->insertHtml(u8"<span style='color:red'> کوئی مانوس بحر نہیں مل سکی </span> ");
                ui->textEdit->insertPlainText("\n");
                ui->textEdit->insertHtml(u8"<span style= 'color:#5900b3'> نزدیک ترین بحر کے ارکان : </span>");
                ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ meter_value);
                //                ui->textEdit->insertPlainText("\n");
                //                ui->textEdit->insertHtml(u8"<span style= 'color:#5900b3'>الفاظ : </span>");
                //				ui->textEdit->insertHtml(u8"(");

                //			 if(meter_value.size()<original_weight.size())
                //			 {
                //				for(int i=0; i<original_weight.size(); ++i)
                //				{
                //						if(i<meter_value.size())
                //						{
                //							if(original_weight[i] == meter_value[i])
                //							{
                //								ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ original_weight[i]);
                //							}
                //							else
                //							{
                //								 ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>"+ original_weight[i]);
                //							}
                //						}
                //						else
                //						{
                //							ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>"+ original_weight[i]);
                //						}
                //				}
                //			 }
                //             else
                //             {
                //			   for(int i=0; i<original_weight.size(); ++i)
                //			   {
                //				 if(original_weight[i] == meter_value[i])
                //				 {
                //					 ui->textEdit->insertHtml(u8"<span style= 'color:red'></span>" + original_weight[i]);
                //				 }
                //				 else
                //				 {
                //					  ui->textEdit->insertHtml(u8"<span style= 'color:red'> </span>" + original_weight[i]);
                //				 }

                //			   }
                //				ui->textEdit->insertPlainText(")");

                //            }
            }
            else
                ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");

        }



        auto meters_find_iterator = Names_map.find(accumulated_weights[index].bin.toStdWString());

        if (meters_find_iterator != Names_map.end())
        {
            ui->textEdit->insertPlainText(u8"\nبحر: ");
            QString name_value = QString::fromStdWString(meters_find_iterator->second);
            QString additional_zuhaf = "";
            if(tasbeegh_o_azala)
            {
                QString rukn = accumulated_weights[index].bin.mid(accumulated_weights[index].bin.size()-4,4);
                if(rukn== u8"0110" && !name_value.contains(u8"مذال"))
                {
                    additional_zuhaf = u8"مذال";
                }
                else if(rukn != u8"0110" && !name_value.contains(u8"مسبغ") && !name_value.contains(u8"مذال"))
                {

                    additional_zuhaf = u8"مسبغ";
                }

            }
            ui->textEdit->insertPlainText(name_value + " " + additional_zuhaf);
        }
        else
        {
            auto it = Names_map.find(key);
            if(it!= Names_map.end())
            {
                QString name_value = QString::fromStdWString(it->second);
                //  ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی مانوس بحر نہیں مل سکی </span>|");
                ui->textEdit->insertPlainText("\n");
                ui->textEdit->insertHtml(u8"<span style='color:#5900b3'>نزدیک ترین بحر کا نام :   </span>");
                ui->textEdit->insertHtml(u8"<span style= 'color:black'></span>"+ name_value);
            }
            //      else {
            //          ui->textEdit->insertHtml(u8"<span style='color:red'>  کوئی بحر نہیں مل سکی </span>|");
            //      }

        }

        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Displaying Names: " << end.count() << "\n";
    }

    void CustomWindow::display_arkans(const QVector<QStringList>& words_murrab_weight_per_line)
    {
        auto start = std::chrono::high_resolution_clock::now();
        int size = words_murrab_weight_per_line.size();

        if (size <= 0)
            return;

        ui->textEdit->insertPlainText(u8"\nتحلیلِ الفاظ: ");


        for (int i = 0; i < size; i++)
        {
            if (words_murrab_weight_per_line[i].size() != 3)
            {
                ui->textEdit->insertHtml(u8"<span style='color:red'>٭</span> ");
                continue;
            }

            const QString weight = words_murrab_weight_per_line[i][2];

            auto arkaan_find_iterator = Arkan_map.find(weight.toStdWString());

            if (arkaan_find_iterator != Arkan_map.end())
            {
                const QString rukan = QString::fromStdWString(arkaan_find_iterator->second);

                bool has_multiple_weights = has_different_weights(words_murrab_weight_per_line[i][0]);

                if (has_multiple_weights)
                {
                    ui->textEdit->insertHtml(u8"<span style='color:#5900b3'>" + rukan + u8"</span> ");
                }
                else
                {
                    ui->textEdit->insertPlainText(rukan + " ");
                }
            }
            else
            {
                ui->textEdit->insertHtml(u8"<span style='color:red'>٭</span> ");
            }
        }

        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Displaying Arkans: " << end.count() << "\n";
    }

    QString get_closest_meter(const QVector<QVector<Accumulated_Weight>>& accumulated_weights_all_lines)
    {

        QMap<QString, int> closest_meter_counts;

        int max_count = INT_MIN;
        QString max_closest_meter;

        for (auto&accumulated_weights : accumulated_weights_all_lines )
        {
            QString closest_meter;

            unsigned int distance = 0;
            int count = 0;

            for(auto&i : accumulated_weights)
            {

                for(auto&j: Meter_map)
                {

                    unsigned int value = levenshteinDist(j.first,i.bin.toStdWString());
                    if( value<distance || count ==0)
                    {
                        distance=value;
                        closest_meter = QString::fromStdWString(j.first);

                        closest_meter_counts[closest_meter]++;

                        ++count;
                    }
                }
            }
        }

        for (auto it = closest_meter_counts.begin(); it != closest_meter_counts.end(); it++)
        {
            if (it.value() > max_count)
            {
                max_count = it.value();
                max_closest_meter = it.key();
            }
        }

        return max_closest_meter;
    }

    QVector<QString> max_count_meter(const QVector<QStringList>& found_meters, QVector<QVector<Accumulated_Weight>> accumulated_weights_all_lines)
    {
        QMap<QString, int> meters_count_in_line;

        QVector<QString> max_meters = {};

        int count = 0;

        for (int i = 0; i < found_meters.size(); i++)
        {
            for (int j = 0; j < found_meters[i].size(); j++)
            {
                meters_count_in_line[found_meters[i][j]]++;
            }
        }

        for (auto it = meters_count_in_line.begin(); it != meters_count_in_line.end(); it++)
        {
            if (it.value() > count)
            {
                count = it.value();

                if (max_meters.isEmpty())
                    max_meters.push_back(it.key());
                else
                    max_meters.front() = it.key();
            }
        }

        if (count == 0)
        {
            QString closest_meter = get_closest_meter(accumulated_weights_all_lines);

            max_meters.push_back(closest_meter);
        }

        QString max_found_meter = max_meters.front();

        if(max_found_meter == "10110101110101110101110" || max_found_meter == "10110101110101110101010" || max_found_meter == "1110101110101110101110" || max_found_meter == "1110101110101110101010" )
        {
            if(max_found_meter == "10110101110101110101110" )
            {
                max_meters.push_back("10110101110101110101010");
                max_meters.push_back("1110101110101110101110");
                max_meters.push_back("1110101110101110101010");
            }
            else if(max_found_meter == "10110101110101110101010" )
            {
                max_meters.push_back("10110101110101110101110");
                max_meters.push_back("1110101110101110101110");
                max_meters.push_back("1110101110101110101010");
            }
            else if(max_found_meter == "1110101110101110101110" )
            {
                max_meters.push_back("10110101110101110101010");
                max_meters.push_back("10110101110101110101110");
                max_meters.push_back("1110101110101110101010");
            }
            else if(max_found_meter == "1110101110101110101010" )
            {
                max_meters.push_back("10110101110101110101010");
                max_meters.push_back("1110101110101110101110");
                max_meters.push_back("10110101110101110101110");
            }
        }

        else if(max_found_meter == "10110101101101110" || max_found_meter == "10110101101101010" || max_found_meter == "1110101101101110" || max_found_meter == "1110101101101010" )
        {
            if(max_found_meter == "10110101101101110" )
            {
                max_meters.push_back("10110101101101010");
                max_meters.push_back("1110101101101110");
                max_meters.push_back("1110101101101010");
            }
            else if(max_found_meter == "10110101101101010" )
            {
                max_meters.push_back("10110101101101110");
                max_meters.push_back("1110101101101110");
                max_meters.push_back("1110101101101010");
            }
            else if(max_found_meter == "1110101101101110" )
            {
                max_meters.push_back("1110101101101010");
                max_meters.push_back("10110101101101110");
                max_meters.push_back("10110101101101010");
            }
            else if(max_found_meter == "1110101101101010" )
            {
                max_meters.push_back("1110101101101110");
                max_meters.push_back("10110101101101110");
                max_meters.push_back("10110101101101010");
            }
        }

        else if(max_found_meter == "10110101110101110" || max_found_meter == "10110101110101010" || max_found_meter == "1110101110101110" || max_found_meter == "1110101110101010" )
        {
            if(max_found_meter == "10110101110101110" )
            {
                max_meters.push_back("10110101110101010");
                max_meters.push_back("1110101110101110");
                max_meters.push_back("1110101110101010");
            }
            else if(max_found_meter == "10110101101101010" )
            {
                max_meters.push_back("10110101110101110");
                max_meters.push_back("1110101110101110");
                max_meters.push_back("1110101110101010");
            }
            else if(max_found_meter == "1110101101101110" )
            {
                max_meters.push_back("10110101101101010");
                max_meters.push_back("10110101110101110");
                max_meters.push_back("1110101110101010");
            }
            else if(max_found_meter == "1110101101101010" )
            {
                max_meters.push_back("1110101101101110");
                max_meters.push_back("10110101110101110");
                max_meters.push_back("10110101101101010");
            }
        }

        else if(max_found_meter == "1101101110101101101110" || max_found_meter == "1101101110101101101010")
        {
            if(max_found_meter == "1101101110101101101110" )
            {
                max_meters.push_back("1101101110101101101010");
            }
            else if(max_found_meter == "1101101110101101101010" )
            {
                max_meters.push_back("1101101110101101101110");
            }

        }

        else if(max_found_meter == "1010111011011010" || max_found_meter == "1010101011011010")
        {
            if(max_found_meter == "1010111011011010" )
            {
                max_meters.push_back("1010101011011010");
            }
            else if(max_found_meter == "1010101011011010" )
            {
                max_meters.push_back("1010111011011010");
            }
        }

        return max_meters;
    }

    QStringList CustomWindow::get_unrecognized_words(const QStringList& user_entered_line, const QVector<QStringList>& words_murrabs_weight_per_line)
    {
        QStringList unrecognized_words;

        for (int i = 0; i < words_murrabs_weight_per_line.size(); i++)
        {
            if (words_murrabs_weight_per_line[i].size() != 3)
            {
                unrecognized_words.push_back(user_entered_line[i]);
            }
        }

        return unrecognized_words;
    }
    void CustomWindow::execute_taqti_program()
    {
        QVector<QStringList> user_entered_lines = get_user_input();

        QVector<QStringList> words_murrabs_weights_per_line = {};

        ui->textEdit->clear();
        ui->textEdit->setHtml(textedit_html);

        for (auto& line: user_entered_lines)
        {
            for(auto& i:line)
            {
                ui->textEdit->insertPlainText(i);
                ui->textEdit->insertPlainText(" ");
            }

            words_murrabs_weights_per_line = get_murrab_weight(line);
            display_meters(words_murrabs_weights_per_line);
            display_arkans(words_murrabs_weights_per_line);

            if(user_entered_lines.size()>1 && line!=user_entered_lines.back())
            {
                ui->textEdit->insertPlainText("\n\n");
            }

        }
    }

    void CustomWindow::execute_islah_program()
    {
        QVector<QVector<Accumulated_Weight>> accumulated_weights = {};

        QVector<QStringList> user_entered_lines = get_user_input();

        QVector<QStringList> words_murrabs_weights_per_line = {};

        QVector<QVector<QStringList>> words_murrabs_weights_all_lines;

        QVector<QStringList> all_matched_meters;


        for (auto& line: user_entered_lines)
        {

            words_murrabs_weights_per_line = get_murrab_weight(line);

            accumulated_weights.push_back(get_accumulated_weight(words_murrabs_weights_per_line));

            words_murrabs_weights_all_lines.push_back(words_murrabs_weights_per_line);

            if (!accumulated_weights.isEmpty() && !accumulated_weights.back().isEmpty() && accumulated_weights.back().back().has_meter)
                all_matched_meters.push_back(get_matched_meters(accumulated_weights.back()));
        }

        ui->textEdit->clear();
        ui->textEdit->setHtml(textedit_html);

        QVector<QString> most_matched_meters = {};

        if (!accumulated_weights.isEmpty())
            most_matched_meters = max_count_meter(all_matched_meters, accumulated_weights);

        bool has_a_valid_verse = false;

        for (int i = 0; i < user_entered_lines.size(); i++)
        {
            ui->textEdit->insertPlainText(" ");

            if (accumulated_weights[i].isEmpty() || !accumulated_weights[i].back().has_meter)
            {
                ui->textEdit->insertHtml( accumulate(user_entered_lines[i], " ") + u8"<br/> ");

                QStringList unrecognized_words = get_unrecognized_words(user_entered_lines[i], words_murrabs_weights_all_lines[i]);

                for (int i = 0; i < unrecognized_words.size(); i++)
                {
                    if (i != 0) ui->textEdit->insertPlainText(u8"،");

                    ui->textEdit->insertPlainText(unrecognized_words[i]);
                }

                ui->textEdit->insertPlainText(": ");

                QString errorMessage = u8"ان الفاظ کی شناخت نہ کی جا سکی";

                ui->textEdit->insertHtml(u8"<span style='color:red'>" + errorMessage + u8"</span> ");

                display_arkans(words_murrabs_weights_all_lines[i]);

                ui->textEdit->insertPlainText("\n");

                continue;
            }

            Accumulated_Weight aw = islah(accumulated_weights[i], most_matched_meters);

            auto meter_name_it = Names_map.find(islah_best_meter_bin.toStdWString());
            auto meter_it = Meter_map.find(islah_best_meter_bin.toStdWString());

            // Displaying the meters here
            if (meter_name_it != Names_map.end())
            {
                ui->textEdit->insertHtml(u8"<span style='color:rgb(112, 112, 177);'>" + QString::fromStdWString(meter_name_it->second) + u8"</span><br/> ");
            }
            if (meter_it != Meter_map.end())
            {
                ui->textEdit->insertHtml(u8"<span style='color:grey'>" + QString::fromStdWString(meter_it->second) + u8"</span><br/> ");
            }

            for (int j = 0; j < aw.weights.size(); j++)
            {
                if (aw.rejected[j])
                {
                    ui->textEdit->insertHtml(u8"<span style='color:red'>" + aw.words[j] + u8"</span> ");
                }
                else
                {
                    ui->textEdit->insertHtml(u8"<span style='color:#2e7d32 '>" + aw.words[j] + u8"</span> ");
                }
            }

            ui->textEdit->insertPlainText("\n");

            for (int j = aw.weights.size() - 1; j >= 0; j--)
            {
                if (aw.rejected[j])
                {
                    ui->textEdit->insertHtml(u8"<span style='color:red'>" + aw.weights[j] + u8"</span> ");
                }
                else
                {
                    ui->textEdit->insertHtml(u8"<span style='color:#009688'>" +  aw.weights[j] + u8"</span> ");
                }
            }
            display_arkans(words_murrabs_weights_all_lines[i]);

            ui->textEdit->insertPlainText("\n\n");
            // QString meter = Meter_map.find()
            if (!has_a_valid_verse) has_a_valid_verse = true;
        }

        if (has_a_valid_verse && !most_matched_meters.empty())
        {
            if(most_matched_meters.size()==1)
                ui->textEdit->insertHtml(u8"<br/> اشعار کا موازنہ اس بحر سے کیا گیا ہے:  <br/><br/> ");
            else {
                ui->textEdit->insertHtml(u8"<br/> اشعار کا موازنہ ان بحور سے کیا گیا ہے:  <br/><br/> ");
            }

            for (auto& meter: most_matched_meters)
            {
                auto it = Names_map.find(meter.toStdWString());
                auto it2 = Meter_map.find(meter.toStdWString());

                ui->textEdit->insertHtml(((it != Names_map.end()) ? (QString::fromStdWString(it->second) + " (" + u8"<span style='color:#ad1457 '>"+ QString::fromStdWString(it2->second) + ") <br/> " ): "") + u8"</span> ");
            }
        }

        ui->textEdit->moveCursor(QTextCursor::Start);

    }

    //  if (has_a_valid_verse)
    //  {
    //      ui->textEdit->insertPlainText(u8"\n اشعار کا موازنہ اس بحر سے کیا گیا ہے:  " );
    //    ui->textEdit->insertHtml(u8"<span style='color:#ad1457'>" + ((it != Names_map.end()) ? (QString::fromStdWString(it->second) +u8"</span> " + " (" + u8"<span style='color:#ff0066  '>" + QString::fromStdWString(it2->second) + u8"</span> "+  ")" ): ""));

    //  }
    //  ui->textEdit->moveCursor(QTextCursor::Start);

    //}

    void CustomWindow::on_pushButton_clicked()
    {
        auto start = std::chrono::high_resolution_clock::now();

        if (mode == ProgramMode::TAQTI)
        {
            execute_taqti_program();
        }
        else if (mode == ProgramMode::ISLAH)
        {
            execute_islah_program();
        }

        std::chrono::duration<double> end = std::chrono::high_resolution_clock::now() - start;

        QTextStream(stdout) << "Time elapsed: " << end.count() << "\n ---------------------------- \n";
    }

    void CustomWindow::on_pushButton_2_clicked()
    {
        ui->textEdit->clear();
        ui->textEdit->setHtml(textedit_html);
    }

    void CustomWindow::on_taqtiButton_clicked()
    {
        if (mode == ProgramMode::ISLAH)
        {
            prev_islah_text = ui->textEdit->toHtml();
            ui->textEdit->setHtml(prev_taqti_text);
        }

        mode = ProgramMode::TAQTI;
        ui->taqtiButton->setStyleSheet(taqti_but_stylesheet + "background-color: #b0bec5;");
        ui->islahButton->setStyleSheet(islah_but_stylesheet + "font-weight: normal; border-bottom: none;");

    }

    void CustomWindow::on_islahButton_clicked()
    {
        if (mode == ProgramMode::TAQTI)
        {
            prev_taqti_text = ui->textEdit->toHtml();
            ui->textEdit->setHtml(prev_islah_text);
        }

        mode = ProgramMode::ISLAH;

        ui->islahButton->setStyleSheet(islah_but_stylesheet + "background-color: #b0bec5;");
        ui->taqtiButton->setStyleSheet(taqti_but_stylesheet + "font-weight: normal; border-bottom: none;");
    }

