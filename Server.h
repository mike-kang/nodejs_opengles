#include <thread>

class CServer {
public:
    void start();
    void stop();
    void setOnMessage(void (*)(char* msg, int length));

private:
    void init();
    void loop();

    int m_sock;
    bool m_running;
    std::thread* m_thread;
    void (*m_fn)(char* msg, int length);
};