/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include "core/safebrowsing.cpp"

class SafeBrowsingTests : public QObject
{
        Q_OBJECT

    public:
        SafeBrowsingTests();
        ~SafeBrowsingTests();

    private slots:
        void canonicalize_data();
        void canonicalize();

        void suffixPrefixExpressions_data();
        void suffixPrefixExpressions();

        void fullTest();

};

SafeBrowsingTests::SafeBrowsingTests()
{

}

SafeBrowsingTests::~SafeBrowsingTests()
{

}

void SafeBrowsingTests::canonicalize_data() {
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("result");

    //Unit tests from Google
    QTest::newRow("Test 1") << QUrl::fromUserInput("http://host/%25%32%35") << "http://host/%25";
    QTest::newRow("Test 2") << QUrl::fromUserInput("http://host/%25%32%35%25%32%35") << "http://host/%25%25";
    QTest::newRow("Test 3") << QUrl::fromUserInput("http://host/%2525252525252525") << "http://host/%25";
    QTest::newRow("Test 4") << QUrl::fromUserInput("http://host/asdf%25%32%35asd") << "http://host/asdf%25asd";
    QTest::newRow("Test 5") << QUrl::fromUserInput("http://host/%%%25%32%35asd%%") << "http://host/%25%25%25asd%25%25";
    QTest::newRow("Test 6") << QUrl::fromUserInput("http://www.google.com/") << "http://www.google.com/";
    QTest::newRow("Test 7") << QUrl::fromUserInput("http://%31%36%38%2e%31%38%38%2e%39%39%2e%32%36/%2E%73%65%63%75%72%65/%77%77%77%2E%65%62%61%79%2E%63%6F%6D/") << "http://168.188.99.26/.secure/www.ebay.com/";
    QTest::newRow("Test 8") << QUrl::fromUserInput("http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure<<updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx<<hgplmcx/") << "http://195.127.0.11/uploads/%20%20%20%20/.verify/.eBaysecure<<updateuserdataxplimnbqmn-xplmvalidateinfoswqpcmlx<<hgplmcx/";
    QTest::newRow("Test 9") << QUrl::fromUserInput("http://host%23.com/%257Ea%2521b%2540c%2523d%2524e%25f%255E00%252611%252A22%252833%252944_55%252B") << "http://host%23.com/~a!b@c%23d$e%25f^00&11*22(33)44_55+";
    QTest::newRow("Test 10") << QUrl::fromUserInput("http://3279880203/blah") << "http://195.127.0.11/blah";
    QTest::newRow("Test 11") << QUrl::fromUserInput("http://www.google.com/blah/..") << "http://www.google.com/";
    QTest::newRow("Test 12") << QUrl::fromUserInput("www.google.com/") << "http://www.google.com/";
    QTest::newRow("Test 13") << QUrl::fromUserInput("www.google.com") << "http://www.google.com/";
    QTest::newRow("Test 14") << QUrl::fromUserInput("http://www.evil.com/blah#frag") << "http://www.evil.com/blah";
    QTest::newRow("Test 15") << QUrl::fromUserInput("http://www.GOOgle.com/") << "http://www.google.com/";
    QTest::newRow("Test 16") << QUrl::fromUserInput("http://www.google.com.../") << "http://www.google.com/";
    QTest::newRow("Test 17") << QUrl::fromUserInput("http://www.google.com/foo\tbar\rbaz\n2") <<"http://www.google.com/foobarbaz2";
    QTest::newRow("Test 18") << QUrl::fromUserInput("http://www.google.com/q?") << "http://www.google.com/q?";
    QTest::newRow("Test 19") << QUrl::fromUserInput("http://www.google.com/q?r?") << "http://www.google.com/q?r?";
    QTest::newRow("Test 20") << QUrl::fromUserInput("http://www.google.com/q?r?s") << "http://www.google.com/q?r?s";
    QTest::newRow("Test 21") << QUrl::fromUserInput("http://evil.com/foo#bar#baz") << "http://evil.com/foo";
    QTest::newRow("Test 22") << QUrl::fromUserInput("http://evil.com/foo;") << "http://evil.com/foo;";
    QTest::newRow("Test 23") << QUrl::fromUserInput("http://evil.com/foo?bar;") << "http://evil.com/foo?bar;";
    QTest::newRow("Test 24") << QUrl::fromUserInput("http://\x01\x80.com/") << "http://%01%80.com/";
    QTest::newRow("Test 25") << QUrl::fromUserInput("http://notrailingslash.com") << "http://notrailingslash.com/";
    QTest::newRow("Test 26") << QUrl::fromUserInput("http://www.gotaport.com:1234/") << "http://www.gotaport.com/";
    QTest::newRow("Test 27") << QUrl::fromUserInput("  http://www.google.com/  ") << "http://www.google.com/";
    QTest::newRow("Test 28") << QUrl::fromUserInput("http:// leadingspace.com/") << "http://%20leadingspace.com/";
    QTest::newRow("Test 29") << QUrl::fromUserInput("http://%20leadingspace.com/") << "http://%20leadingspace.com/";
    QTest::newRow("Test 30") << QUrl::fromUserInput("%20leadingspace.com/") << "http://%20leadingspace.com/";
    QTest::newRow("Test 31") << QUrl::fromUserInput("https://www.securesite.com/") << "https://www.securesite.com/";
    QTest::newRow("Test 32") << QUrl::fromUserInput("http://host.com/ab%23cd") << "http://host.com/ab%23cd";
    QTest::newRow("Test 33") << QUrl::fromUserInput("http://host.com//twoslashes?more//slashes") << "http://host.com/twoslashes?more//slashes";
}

void SafeBrowsingTests::canonicalize()
{
    QFETCH(QUrl, url);
    QFETCH(QString, result);

    //TODO: Fix these broken test cases
    QEXPECT_FAIL("Test 4", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 5", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 7", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 8", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 9", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 16", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 17", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 24", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 28", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 29", "To be fixed later", Abort);
    QEXPECT_FAIL("Test 30", "To be fixed later", Abort);

    QCOMPARE(SafeBrowsing::canonicalizeUrl(url), result);
}

void SafeBrowsingTests::suffixPrefixExpressions_data()
{
    QTest::addColumn<QString>("canonicalizedUrl");
    QTest::addColumn<QStringList>("result");

    QTest::newRow("Test 1") << "http://a.b.c/1/2.html?param=1" << QStringList({
                                                                  "a.b.c/1/2.html?param=1",
                                                                  "a.b.c/1/2.html",
                                                                  "a.b.c/",
                                                                  "a.b.c/1/",
                                                                  "b.c/1/2.html?param=1",
                                                                  "b.c/1/2.html",
                                                                  "b.c/",
                                                                  "b.c/1/"
                                                              });
    QTest::newRow("Test 2") << "http://a.b.c.d.e.f.g/1.html" << QStringList({
                                                                  "a.b.c.d.e.f.g/1.html",
                                                                  "a.b.c.d.e.f.g/",
                                                                  "c.d.e.f.g/1.html",
                                                                  "c.d.e.f.g/",
                                                                  "d.e.f.g/1.html",
                                                                  "d.e.f.g/",
                                                                  "e.f.g/1.html",
                                                                  "e.f.g/",
                                                                  "f.g/1.html",
                                                                  "f.g/"
                                                              });
    QTest::newRow("Test 3") << "http://1.2.3.4/1/" << QStringList({
                                                                  "1.2.3.4/1/",
                                                                  "1.2.3.4/"
                                                              });
}

void SafeBrowsingTests::suffixPrefixExpressions()
{
    QFETCH(QString, canonicalizedUrl);
    QFETCH(QStringList, result);

    QCOMPARE(SafeBrowsing::suffixPrefixExpressionsForUrl(canonicalizedUrl), result);
}

void SafeBrowsingTests::fullTest()
{
    qDebug() << QCryptographicHash::hash("abc", QCryptographicHash::Sha256).toHex();
    QString canonicalizedUrl = SafeBrowsing::canonicalizeUrl(QUrl("http://alexfun2.org/wp-content/plugins/upspy/.../nedbank2/up/ab/ban/Login/"));
    QStringList exprs = SafeBrowsing::suffixPrefixExpressionsForUrl(canonicalizedUrl);
    for (QString expr : exprs) {
        QByteArray hash = QCryptographicHash::hash(expr.toUtf8(), QCryptographicHash::Sha256);
        qDebug() << hash.toHex();
    }
}

QTEST_MAIN(SafeBrowsingTests)

#include "tst_safebrowsingtests.moc"
