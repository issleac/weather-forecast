#include <iostream>
#include <fstream>

#include "src/HttpsUtil.h"
#include "src/Weather.h"

#include "include/gumbo-parser/Document.h"
#include "include/gumbo-parser/Selection.h"
#include "include/gumbo-parser/Node.h"

void report(std::vector<Weather>& ve) {
    std::ofstream of;
//    of.open("../output/test.csv", std::ios::out | std::ios::trunc);
    of.open("../output/test.xlsx", std::ios::out | std::ios::trunc);
    of<<"日期"<<"\t"<<"星期"<<"\t"<<"天气现象"<<"\t"<<"气温"<<"\t"<<"风向"<<"\t"<<"风力"<<"\n";
//    for(auto v: ve){
//        v.to_execl(of);
//    }
    for(int i=0;i<5;i++)
        ve[i].to_execl(of);
    of.close();
}

std::vector<Weather> html_parser(const std::string& page, const std::string& strFind) {
    CDocument doc;
    doc.parse(page);
    CSelection c = doc.find(strFind);
    std::string date, week, phenomenon, air_temperature, wind_direction, wind_power;
    std::vector<Weather> ve;
    for (int i = 0; i < c.nodeNum(); i++) {
        if (i % 7 == 0) {
            date = c.nodeAt(i).text();
        }
        if (i % 7 == 1) {
            week = c.nodeAt(i).text();
        }
        if (i % 7 == 3) {
            phenomenon = c.nodeAt(i).text();
        }
        if (i % 7 == 4) {
            air_temperature = c.nodeAt(i).text();
        }
        if (i % 7 == 5) {
            wind_direction = c.nodeAt(i).text();
        }
        if (i % 7 == 6) {
            wind_power = c.nodeAt(i).text();
            ve.emplace_back(Weather(date, week, phenomenon, air_temperature, wind_direction, wind_power));
        }
//        printf("%s%c", c.nodeAt(i).text().c_str(), (i % 7 == 6) ? '\n' : ' ');
    }
    return ve;
}


int main() {
    std::string strResponse;
    if (HttpsUtil::getInstance()->getRequest("www.40tianqi.cn", 443, "/shanghai", strResponse) != CODE_SUCCESS) {
        std::cout << "[ERROR] " << "get req error" << std::endl;
        return 0;
    }
    std::cout << "[INFO] " << "get req success" << std::endl;
    std::ofstream of;
    of.open("../output/html.txt", std::ios::out | std::ios::trunc);
    of << strResponse;
    of.close();
    std::vector<Weather> ve = html_parser(strResponse, "html body div div div div div table tbody tr td");
    report(ve);
    return 0;
}
