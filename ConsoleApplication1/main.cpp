#include <thread>
using std::thread;
#include <mutex>
using std::mutex;
#include <iostream>
using std::cout;
using std::endl;
#include <queue>
using std::queue;
#include <string>
using std::string;
using std::to_string;
#include <functional>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#pragma warning(disable:4996)
using std::ref;
using namespace std;

template <typename T>
class BlockingQueue {
private:
	mutex mutex_;
	queue<T> queue_;
public:
	T pop() {
		this->mutex_.lock();
		T value;
		if (!this->queue_.empty())
		{
			value = this->queue_.front();  // undefined behavior if queue_ is empty
										   // may segfault, may throw, etc.
			this->queue_.pop();
		}
		this->mutex_.unlock();
		return value;
	}

	void push(T value) {
		this->mutex_.lock();
		this->queue_.push(value);
		this->mutex_.unlock();
	}

	bool empty() {
		this->mutex_.lock();
		bool check = this->queue_.empty();
		this->mutex_.unlock();
		return check;
	}
};

void fillWorkQueue(BlockingQueue<string>& workQueue) {
	int size = 2;
	for (int i = 0; i < size; i++)
		workQueue.push("C:\\Users\\TriNguyen\\Desktop\\testdata1.pdf");
}
bool ConvertToTIFF(string input, string output, HANDLE* hThread)
{
	bool RetVal = true;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;
	sa.lpSecurityDescriptor = NULL;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	std::string str = "D:\\ToolTest\\pdftotiffcmd.exe -i "+input+" -o "+output;
	static TCHAR processPath[MAX_PATH];
	copy(str.begin(), str.end(), &processPath[0]);
	processPath[str.size()] = _T('\0');


	
	const char *cstr = str.c_str();
	
	size_t size = strlen(cstr) + 1;
	wchar_t* portName = new wchar_t[size];

	size_t outSize;
	mbstowcs_s(&outSize, portName, size, cstr, size - 1);
	LPWSTR ptr = portName;

	TCHAR lpszClientPath[500] = TEXT("D:\\ToolTest\\pdftotiffcmd.exe -i C:\\Users\\TriNguyen\\Desktop\\testdata1.pdf -o D:\\OutPut\\Two");

	if (!CreateProcess(
		NULL,
		processPath,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_PROCESS_GROUP | CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi)
		)
	{
		int checkError= GetLastError();
		return false;
	}
	SetPriorityClass(pi.hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
	hThread[0] = pi.hThread;
	cout << pi.hThread << endl;
	if (ResumeThread(pi.hThread) == -1)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return false;
	}
	DWORD dwEvent = WaitForSingleObject(pi.hProcess, INFINITE);
	switch (dwEvent)
	{
	case WAIT_TIMEOUT:
	{
		RetVal = false;
		break;
	}
	case WAIT_OBJECT_0 + 1:
	{
		RetVal = false;
		break;
	}
	case WAIT_FAILED:
		RetVal = false;
		break;
	}
	Sleep(1);
	TerminateProcess(pi.hProcess, 0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
void doWork(BlockingQueue<string>& workQueue,string check, HANDLE* hThread) {
	while (!workQueue.empty()) {
		ConvertToTIFF(workQueue.pop(),check, hThread);
	}
}

void multiThreaded(HANDLE* hThread) {
	BlockingQueue<string> workQueue;
	//string output1 = "D:\\OutPut\\One";
	//string output1 =   "D:\\OutPut\\Two";
	fillWorkQueue(workQueue);
	thread t1(doWork, ref(workQueue), "D:\\OutPut\\One", hThread);
	thread t2(doWork, ref(workQueue), "D:\\OutPut\\Two", hThread);

//	thread t1(doWork, ref(workQueue));
//	thread t2(doWork, ref(workQueue));
	t1.join();
	t2.join();
	cout << "done\n";

}
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
wstring stringToWstring(const string &inputString)
{
 // Make room for characters
 std::wstring destinationString(inputString.length(), L' ');

 // Copy string to wstring.
 std::copy(inputString.begin(), inputString.end(), destinationString.begin());
 return destinationString;
}
void nhapten(HANDLE* hThread)
{
	string demo = "";
	cout << "Nhap ten cua ban :  " << endl;
	getline(cin, demo);
	cout << hThread[0] << endl;;
	SuspendThread(hThread[0]);
}
//convert file

int main() {
	cout << endl;
	HANDLE* hThread = new HANDLE[3];
	thread callTool(multiThreaded, hThread);
	thread stopTool(nhapten, hThread);
	callTool.join();
	stopTool.join();

	// Multi Threaded
	cout << "multiThreaded\n";
//	multiThreaded();
	cout << endl;
}
