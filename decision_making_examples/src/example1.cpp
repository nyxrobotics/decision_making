
#include <ros/ros.h>
#include <decision_making/SynchCout.h>
#include <decision_making/BT.h>
#include <decision_making/FSM.h>
#include <decision_making/ROSTask.h>
#include <decision_making/DecisionMaking.h>

using namespace std;
using namespace decision_making;

EventQueue mainEventQueue;

FSM(Turnstile)
{
	FSM_STATES
	{
		Locked,
		Unlocked
	}
	FSM_START(Locked);
	FSM_BGN
	{
		FSM_STATE(Locked)
		{
			FSM_TRANSITIONS
			{
				FSM_ON_EVENT(COIN, FSM_NEXT(Unlocked));
				FSM_ON_EVENT(PUSH, FSM_NEXT(Locked));
			}
		}
		FSM_STATE(Unlocked)
		{
			FSM_TRANSITIONS
			{
				FSM_ON_EVENT(COIN, FSM_NEXT(Unlocked));
				FSM_ON_EVENT(PUSH, FSM_NEXT(Locked));
			}
		}
	}
	FSM_END
}

void run_fsm(){
	FsmTurnstile(NULL, &mainEventQueue);
}


void EVENTS_GENERATOR(){
	Event spec[]={"COIN","COIN","PUSH","PUSH", "NOTHING"};
	int i=0;
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	while(true and ros::ok()){
		Event t = spec[i];
		if(t == "NOTHING"){ i=1; t=spec[0]; }else i++;
		cout << endl << t<<" -> ";
		mainEventQueue.riseEvent(t);
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
	mainEventQueue.close();
}

TaskResult tst_mytask(std::string task_address, const FSMCallContext& call_ctx, EventQueue& queue){
	cout<<"[ this my dummy task ]";
	queue.riseEvent(Event("success", call_ctx));
	return TaskResult::SUCCESS();
}

#include <boost/filesystem.hpp>

int main(int a, char** aa){

	ros::init(a, aa, "RosExample");
	ros_decision_making_init(a, aa);

	boost::thread_group threads;

	MapResultEvent::map("MYTASK", 0, "success");
	LocalTasks::registrate("MYTASK", tst_mytask);

	threads.add_thread(new boost::thread(boost::bind(&run_fsm)));
	threads.add_thread(new boost::thread(boost::bind(&EVENTS_GENERATOR)));

	threads.join_all();


	return 0;
}


