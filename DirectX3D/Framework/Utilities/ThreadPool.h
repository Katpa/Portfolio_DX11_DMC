#pragma once

typedef function<void(POINT, POINT, vector<Vector3>*, UINT)> JPSParamEvent;

struct ThreadBuffer
{
	JPSParamEvent func;
	POINT firstParam;
	POINT secondParam;
	vector<Vector3>* pathes;
	UINT index;
};

class ThreadPool : public Singleton<ThreadPool>
{
public:
	explicit ThreadPool();
	~ThreadPool();

	void GetTask(ThreadBuffer buffer);

private:
	void Tasking();

private:
	vector<thread> threads;
	queue<Event> tasks;
	mutex queueMutex;
	condition_variable condition;
	bool stop = false;
};