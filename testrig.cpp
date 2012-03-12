#include "testrig.h"
#include "logger.h"
#include <QDebug>
#include <QMessageBox>

QList<double> floatrange(double i, double j, double k) {
    QList<double> results;

    for (int x = i; x > j; x += k)
	results << x;

    return results;
}

TestRig::TestRig(QString source, bool keep)
{
    m_source = source;
    m_keep = keep;
}

void TestRig::start() {
    QFile srcFile(m_source);

    if (!srcFile.open(QFile::ReadOnly | QFile::Text)) {
	QMessageBox::critical(0, "blind264",
			     tr("Cannot read file %1:\n%2.")
			     .arg(m_source)
			     .arg(srcFile.errorString()));
	return;
    }

    m_avsText.clear();

    // Load .AVS file into memory, but also check for SelectRangeEvery.
    while (!srcFile.atEnd()) {
	QString line(srcFile.readLine());

	if (line.toLower().startsWith("selectrangeevery")) {
	    Logger::log("File " + m_source + " contains a SelectRangeEvery() line. Please remove and try again");
	    return;
	}

	m_avsText.append(line);
    }

    srcFile.close();

    // Find width, height, and frame count
    QProcess *avs2yuv = new QProcess;
    avs2yuv->setProcessChannelMode(QProcess::MergedChannels);
    //args = shlex.split('pipebuf avs2yuv ' + self.source + ' -frames 1 -o - : x264 - --stdin y4m --output ')

    QStringList argList = QString("avs2yuv " + QFileInfo(srcFile).canonicalFilePath() + " -frames 1 -o - : x264 - --stdin y4m --output test.mkv").split(' ');

    avs2yuv->start("./pipebuf", argList);
    avs2yuv->waitForFinished(-1);

    QString avs2yuvOutput = QString(avs2yuv->readAllStandardOutput());

    emit requestPrint("avs2yuv output: ");
    emit requestPrint(avs2yuvOutput);

    // Output
    // C:\DVD\DRHORRIBLE\DHSAB.avs: 716x480, 24000/1001 fps, 60914 frames

    QStringList avs2yuvList = avs2yuvOutput.split(' ');

    int avs2yuvListCount = avs2yuvList.size();
    this->m_frames = avs2yuvList.at(avs2yuvListCount - 2).toInt();

    // determine dimensions (width x height)
    // m_dims.setWidth(n);
    // m_dims.setHeight(n);

    m_params.insert("ref", QString::number(qMin(16, (8388608 / (m_dims.width() * m_dims.height())))));
    m_params.insert("ssim", QString());
}

void TestRig::setRange(int step, int count, int seek) {
    QFile testAvsFile(Logger::logDir() + QDir::separator() + "test.avs");

    if (!testAvsFile.open(QFile::WriteOnly | QFile::Text)) {
	QMessageBox::critical(0, "blind264",
			     tr("Cannot read file %1:\n%2.")
			     .arg(m_source)
			     .arg(testAvsFile.errorString()));
	return;
    }

    QTextStream out(&testAvsFile);

    foreach (QString line, m_avsText)
	out << line;

    if (step != 0 && count != 0) {
	out << QString("\nSelectRangeEvery(" + QString::number(step) + "," + QString::number(count) + "," + QString::number(seek) + ")");
	Logger::log("Setting SelectRangeEvery(" + QString::number(step) + "," + QString::number(count) + "," + QString::number(seek) + ")\n");
    }

    testAvsFile.close();
}

QString TestRig::toString() {
    QString buf;

    foreach (QString key, m_params.keys()) {
	buf.append(" --" + key);

	QString value = m_params.value(key);

	if (!value.isEmpty()) {
	    buf.append(" " + value);
	}
    }

    return buf;
}

void TestRig::override(QString setting, QStringList tests, QString msg) {
    if (tests.contains(setting)) {
	QString override = QInputDialog::getText(0, "Override", "Input a value to use for " + setting + ", or press ENTER to use the computed value.");

	if (!override.isEmpty()) {
	    m_params.insert(setting, override);
	}
    } else {
	QString override;

	while (override.isEmpty()) {
	    override = QInputDialog::getText(0, "Override", "Input a value to use for " + setting + ". " + msg);
	}

	m_params.insert(setting, override);
	Logger::log("User chose " + setting + " " + override);
    }
}

QMap<QString, double> TestRig::testEncode(QString outFile, bool usePrevious, bool verbose) {
    QMap<QString, double> info;

    m_params.remove("output");

    if (usePrevious && m_results.contains(toString())) {
	// moo
	info = m_results.value(toString());
	Logger::log("Using previously-recorded data from the following parameters:\n" + toString());
	Logger::log("\nPreviously-recorded results:\nframes:\t\t" + QString::number(info.value("frames")) + "\nssim:\t\t" +
		    QString::number(info.value("ssim")) + "\ndb:\t\t" + QString::number(info.value("db")) + "\nbitrate:\t\t" +
		    QString::number(info.value("bitrate")) + "\nfps:\t\t" + QString::number(info.value("fps")) + "time:\t\t" +
		    QString::number(info.value("time")) + "\n");
	return info;
    }

    if (m_keep || outFile.startsWith("psy-rd"))
	m_params.insert("output", outFile);	// TODO: fix
    else
	m_params.insert("output", "test.mkv");

    if (verbose) {
	Logger::log("Starting test encode with the following parameters:\n" + toString());
    }

    QString paramString = toString();

    paramString.prepend("pipebuf avs2yuv " + Logger::logDir() + "test.avs -o - : x264 - --stdin y4m");

    QStringList args = paramString.split(' ');

    QTime startTime = QTime::currentTime();

    QProcess *avs2yuv = new QProcess;
    avs2yuv->setProcessChannelMode(QProcess::MergedChannels);
    avs2yuv->start("./pipebuf", args);
    avs2yuv->waitForFinished(-1);

    QString workfile = QString(avs2yuv->readAllStandardOutput());

    QTime endTime = QTime::currentTime();

    info.clear();

    //info['time'] = (end - start)
    //info.insert("time", (endTime - startTime).toString());

    foreach (QString line, workfile) {
	if (line.isEmpty())
	    break;
	else if (line.contains("consecutive B-frames")) {
//	    bframe_list = [float(s[:-1]) for s in line.split()[4:]]
//	    info['bframes'] = [0,bframe_list]
//	    for i in range(len(bframe_list)):
//		if bframe_list[i] >= 1.0: info['bframes'][0] = i
	} else if (line.contains("SSIM Mean")) {
//	    metric_list = line.split()
//	    info['ssim'] = float(metric_list[4][2:])
//	    info['db'] = float(metric_list[5][1:-3])
	} else if (line.startsWith("encoded")) {
	    QStringList rateList = line.split(' ');

	    info.insert("frames", (double)rateList.at(1).toInt());
	    info.insert("fps", rateList.at(3).toDouble());
	    info.insert("bitrate", rateList.at(5).toDouble());
	}
    }

    if (info.isEmpty()) {
	Logger::log("\n" + workfile + "\nTEST ENCODE FAILED");
	return info;
    }

    Logger::log("\nFinished test encode with the following results:\nframes:\t\t" + QString::number(info.value("frames")) + "\nssim:\t\t" +
		QString::number(info.value("ssim")) + "\ndb:\t\t" + QString::number(info.value("db")) + "\nbitrate:\t\t" +
		QString::number(info.value("bitrate")) + "\nfps:\t\t" + QString::number(info.value("fps")) + "time:\t\t" +
		QString::number(info.value("time")) + "\n");


    m_params.remove("output");

    m_results.insert(toString(), info);

    return info;
}

void TestRig::bframes() {
//    self.log('\nRunning bframes test...\n')
//    test = self.test_encode('baseline.mkv')
//    b = test['bframes'][0]
//    self.log('Consecutive bframes used: '+str(test['bframes'][1])+'\n\nblind264 chose bframes = ' + str(b) + '\n')
//    self.set('bframes',b)
//    self.baseline = test
    Logger::log("\nRunning bframes test...\n");
    QMap<QString, double> test = testEncode("baseline.mkv");
    double b = test.value("bframes"); // [0]
    Logger::log("Consecutive bframes used: ");

    m_params.insert("bframes", b);

    m_baseline = test;
}
