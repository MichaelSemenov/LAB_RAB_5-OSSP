#include <iostream>
#include <thread>
#include <mutex>
#include <vector> 
#include <string>
#include <locale>
#include <condition_variable>

using namespace std;

mutex mtx;
mutex test;
bool ready = false;
condition_variable cv;
int counter_producer = 0;
int counter_consumer = 0;

struct Message {
    int id = NULL;
    string text;
};
Message* message;

class MessageQueue {
public:
    MessageQueue(int size) :                         
        messages(size), head(0), tail(0),
        added(0), extracted(0) {}

    void addMessage(Message* message) {               
        //unique_lock<mutex> lock(mtx);
        //unique_lock<mutex> lock_t(test);
        //while (added - extracted == messages.size()) {
        //    not_full.wait(lock);
        //}
        //while (ready) {
        //    cv.wait(lock_t);
        //}
        mtx.lock();
        messages[tail] = message;
        tail = (tail + 1) % messages.size();
        added++;
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(2000));
        //not_empty.notify_one();
    }


    Message* getMessage() {
        //unique_lock<mutex> lock(mtx);
        //unique_lock<mutex> lock_t(test);
        //while (added == extracted) {
        //    not_empty.wait(lock);                                    
        //}
        //while (ready) {
        //    cv.wait(lock_t);
        //}
                
        test.lock();
                message= messages[head];
                head = (head + 1) % messages.size();
                extracted++;
                --added;
        test.unlock(); 
                this_thread::sleep_for(chrono::milliseconds(2000));


        /*not_full.notify_one();*/
        return message;

    }
    ~MessageQueue()
    {
        cout << "Вызван деструктор, программа успешно завершена!" << endl;
    }
    //public для удобства использования!
    vector<Message*> messages;
    int head;
    int tail;
    int added;
    int extracted;
    mutex mtx;
    condition_variable not_empty;
    condition_variable not_full;
};

void producer(MessageQueue* queue, int id) {
    
    while (true)
    {
        if (queue->added == 9) {
            continue;
        }
        
        Message* message = new Message();
        message->id = id;
        message->text = "Message from producer " + to_string(id);
        queue->addMessage(message);
        }


}

void consumer(MessageQueue* queue, int id) {
   // test.lock();
    while (true) {
        if (queue->added == 0) {
            continue;
        }
        Message* message = queue->getMessage();
        cout << "Consumer " << id << " got message #" << message->id
            << " with text: " << message->text << endl;
        delete message;
    }

       // test.unlock();
}

void show_info() {
    cout << "[1] - Создание потока-производителя!" << '\n'
        << "[2] - Создание потока-потребителя!" << '\n'
        << "[3] - Уведичение размера очереди!" << '\n'
        << "[4] - Уменьшение размера очереди!" << '\n'
        << "[5] - Завершение программы!" << '\n'
        << "[6] - Размер очереди после изменения!" << endl;
}

void message_add(MessageQueue& queue) {
    ready = true;
    queue.messages.resize(queue.messages.size() + 1);
    cv.notify_all();
    ready = false;
}

void message_put_away(MessageQueue& queue) {
    ready = true;
    string str = queue.messages.at(queue.messages.size() - 1)->text;
    cout << "Удалена строка из очереди: " << str << endl;
    queue.messages.resize(queue.messages.size() - 1);
   /* cv.notify_all();*/
    ready = false;
}

int main() {
	setlocale(LC_ALL, "rus");
	cout << "Проверка работы лабараторной работы №5" << endl;
	bool flag = true;
	int a;
    MessageQueue queue(10);
	vector<thread> vc_thread;
	show_info();
	do {
		std::cout << "Ваш вариант: ";
		cin >> a;
        switch (a) {
        case 1: {
            thread th(producer, &queue, counter_producer++);
            th.detach();
            break;
        }
        case 2: {
            thread th(consumer, &queue, counter_consumer++);
            th.detach();
            break;
        }
        case 3: {
            cout << "Очередь увеличилась на один элемент " << endl;
            message_add(queue);
            break;
        }
        case 4: {
            cout << "Очередь уменьшилась на один элемент " << endl;
            message_put_away(queue);
            break;
        }
        case 5: {
            flag = false;
            break;
        }
        case 6: {
            cout << "Проверка изменения очереди сообщения" << endl;
            cout << "Размер очереди: " << queue.messages.size() << endl;
            break;
        }
		default:
		{
			cout << "Выбран неверный вариант! Повторите попытку..." << endl;
			break;
		}
		}
	} while (flag);
	cout << "Программа успешно завершена! " << endl;
	return 0;
}