#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->newButton,SIGNAL(clicked()),this,SLOT(on_newButton_clicked()));
    //QObject::connect(ui->browserFileToolButton,SIGNAL(clicked()),this,SLOT(on_browserFileToolButton_clicked()));
    QObject::connect(ui->procListWidget,SIGNAL(itemSelectionChanged()),this,SLOT(itemTextSolt()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_newButton_clicked()
{
    HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(INVALID_HANDLE_VALUE == hSnapProcess)
    {
        QMessageBox::information(this,"ERROR","Error when create snapshot !");
        return;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof (pe);

    if(FALSE == Process32First(hSnapProcess,&pe))
    {
        QMessageBox::information(this,"ERROR","Error when Process32First !");
        return;
    }

    while(Process32Next(hSnapProcess,&pe))
    {
        //WCHAR ProcessName = pe.szExeFile;
        QString ProcessName = QString::fromWCharArray(pe.szExeFile);
        //ui->procTextEdit->append(ProcessName);
        //QString ProcessID = pe.th32ProcessID;
        int ProcessID = (int)pe.th32ProcessID;
        ui->procListWidget->addItem("PID["+QString::number(ProcessID) +"]    "+ProcessName);
        //qDebug()<<ui->procListWidget->currentItem()->text();

    }
}

void MainWindow::on_browserFileToolButton_clicked()
{
    dllName = QFileDialog::getOpenFileName(this,"open file",QDir::currentPath());
    if(dllName.isEmpty())
    {
        QMessageBox::information(this,"ERROR","Open Dll Failed , Please Select Again !");
    }
    ui->dllNameLineEdit->setText(dllName);
}

void MainWindow::itemTextSolt()
{
    //qDebug()<<ui->procListWidget->currentItem()->text();
    QString ProInfo = ui->procListWidget->currentItem()->text();
    //QStringList ProInfoList = ProInfo.split("[");
    QString stProID = ProInfo.split("[").at(1).split("]").at(0);
    //qDebug()<<stProID.toInt();
    //qDebug()<<dllName;
    PrcoID=(DWORD)stProID.toDouble();

}

void MainWindow::on_injectButton_clicked()
{
    HANDLE hProHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,PrcoID);
    //qDebug()<<hProHandle;
    if(hProHandle == NULL)
    {
        QMessageBox::information(this,"ERROR","Get PROCESS HANDLE FAILED !");
        return;
    }
    LPVOID IpAdrr = VirtualAllocEx(hProHandle,NULL,0x1000,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

    if(IpAdrr == NULL)
    {
        QMessageBox::information(this,"ERROR","Get IPADRR FAILED !");
        return;
    }
    //qDebug()<<IpAdrr;
    char dllNameBuf[MAX_PATH] = {NULL};
    strcpy(dllNameBuf,dllName.toLocal8Bit().data());
    //qDebug()<<*dllNameBuf;
    bool ok = WriteProcessMemory(hProHandle,IpAdrr,dllNameBuf,strlen(dllNameBuf),NULL);
    if(!ok)
    {
        QMessageBox::information(this,"ERROR","Wirte dll name failed !");
    }
    //qDebug()<<strlen(dllNameBuf)+1;
    //qDebug()<<dllNameBuf;
    //HMODULE hModule = LoadLibrary(TEXT("kernel32"));
    //qDebug()<<hModule;
    //LPTHREAD_START_ROUTINE pfThread = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule,"LoadLibraryA");
    //FARPROC  pfThread = GetProcAddress(GetModuleHandleA("Kernel32.dll"),"LoadLibraryA");
    LPTHREAD_START_ROUTINE pfThread = (LPTHREAD_START_ROUTINE)0x74F1A270;

//    QLibrary mylib("C:\\Windows\\System32\\kernel32.dll");
//    if(mylib.load())
//    {
//        LPFNDLLFUN hLoadLibrary = (LPFNDLLFUN)mylib.resolve("LoadLibraryA");
//        qDebug()<<hLoadLibrary;
//    }

    DWORD dwThreadID;
    HANDLE hThread = CreateRemoteThread(hProHandle,NULL,0,(LPTHREAD_START_ROUTINE)pfThread,IpAdrr,0,&dwThreadID);
    //qDebug()<<dwThreadID;
    //qDebug()<<pfThread;
    WaitForSingleObject(hThread,INFINITE);
    VirtualFreeEx(hProHandle,IpAdrr,0x1000,MEM_RELEASE);
    QMessageBox::information(this,"Success","Dll run over !");




}

void MainWindow::on_uninjectButton_clicked()
{
    qDebug()<<PrcoID;
    QStringList dllnameList = dllName.split("/");
    //qDebug()<<dllnameList.last();
    QString dllname2 = dllnameList.last();
    char dllNameBuf[MAX_PATH] = {NULL};
    strcpy(dllNameBuf,dllname2.toLocal8Bit().data());
    //qDebug()<<dllNameBuf;


    HANDLE hSnap =CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PrcoID);

    MODULEENTRY32 me32 = {0};
    me32.dwSize = sizeof (me32);
    BOOL bRet = Module32First(hSnap,&me32);
    while(bRet)
    {
//         if(qstrcmp((char*)me32.szExePath,dllNameBuf) == 0)
//         {
//               QMessageBox::information(this,"find","ok");
//         }
//         bRet = Module32Next(hSnap,&me32);
        qDebug()<<QString::fromWCharArray(me32.szModule);
        bRet = Module32Next(hSnap,&me32);
    }

    CloseHandle(hSnap);


}
