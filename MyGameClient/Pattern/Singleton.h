#pragma once

template<class T, class Deleter>
class CSingleton
{
public:
	static T& getInstance()
	{
		std::call_once(_flag, []() {
			CSingleton<T, Deleter>::_uinst.reset(new T);
			char name[256] = { 0 };
			sprintf_s(name, "%s Instantiated\n", typeid(T).name());
			});
		return *_uinst;
	}
	virtual void Init() = 0;
	virtual void Update() = 0;

	CSingleton() {};
	virtual ~CSingleton() {};
	// 복사 생성자와 대입 연산자는 사용하지 못하도록 삭제
	CSingleton(const CSingleton&) = delete; 
	CSingleton(CSingleton&&) noexcept = delete; 
	CSingleton& operator=(const CSingleton&) = delete;
	CSingleton& operator=(CSingleton&&) noexcept = delete;
private:
	static std::once_flag _flag;
	static std::unique_ptr<T, Deleter> _uinst;
};

template <class T, class Deleter>
std::once_flag CSingleton<T, Deleter>::_flag;

template <class T, class Deleter>
std::unique_ptr<T, Deleter> CSingleton<T, Deleter>::_uinst = nullptr;
