#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "clusterInterface.h"
using namespace std;

void appendJson(ImgCluster::Benchmark& benchmark);

void printBenchMark(ImgCluster::Benchmark& benchmark){
    cout<<"[Results]"<<endl;
    cout<<"Comparisons:"<<benchmark.compareCnt<<endl;
    cout<<"Max Memory:"<<benchmark.maxNodes<<endl;
    cout<<"Deviation:"<<benchmark.deviation<<endl;
    cout<<"Elapsed Time:"<<benchmark.elapsed<<endl;
    cout<<"Clusters:"<<endl;
    for(ImgCluster::ImageCluster c:benchmark.clusters){
        cout<<"->Pos "<<c.pos.x<<","<<c.pos.y<<endl;
        cout<<"->Cnt "<<c.count<<endl;
        cout<<"->Representative "<<c.repr.fileName<<endl;
    }
    appendJson(benchmark);
}

int main() {
    ifstream file("imageList.csv");
    string line;
    int sWidth, sHeight;
    cout<<"Reading from list of Images.. (imageList.csv)"<<endl;
    ImgCluster::Images images;
    if (!file.is_open()) {
        cerr << "Error: File could not be opened" << endl;
        return 1;
    }

    while (getline(file, line)) {
        string str1;
        int val1, val2;
        istringstream ss(line);
        if (getline(ss, str1, ',') && (ss >> val1) && (ss >> val2)) {
            ImgCluster::ImageNode t;
            t.pos.x = val1;
            t.pos.y = val2;
            t.fileName = str1;
            images.push_back(t);
        }
    }
    cout<<images.size()<<" images detected."<<endl;
    file.close();

    cout<<"Screen Width:"; cin>>sWidth;
    cout<<"Screen Height:"; cin>>sHeight;

    while(1){
        int mode;
        ImgCluster::ClusterAlgorithm algol;
        cout<<"Select Algorithm: 1(Kmeans) 2(Dbscan) 3(RandomForest):";
        cin>>mode;

        switch(mode){
            //set algol to respective algorithm class
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
        }
        auto initBench = algol.init(images, sWidth, sHeight);
        printBenchMark(initBench);

        while(1) {
            int x, y, w, h;
            cout << "Enter <X> <Y> <Width> <Height>(q to break):";
            string input;
            cin >> input;
            if (input == "q") break;
            stringstream ss(input);
            ss >> x;
            cin >> y >> w >> h;

            ImgCluster::Rectangle screen(x, y, w, h);
            auto iterBench = algol.iterate(screen);
            printBenchMark(iterBench);
        }
    }
    return 0;
}
