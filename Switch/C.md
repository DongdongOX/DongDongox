定时器和信号量

```c
POS_VOID l3RefreshTimer(POS_VOID *arg)
{
    giveL3AgingSem();
}

POS_VOID l3AgingProcess(){
    while(1)
    {    
        takeL3AgingSem();   
        work();    
    }
} 

gL3TimerId = PosCreateRTimer((POS_TIMERFUNPTR)l3RefreshTimer, 0, iAgeTime*1000, POS_TIMER_FOREVER);

gL3AgingTaskId = PosCreateTask(60, "tL3IntfAge", (POSTASKFUNCPTR)l3AgingProcess, 0, 32*1024);  
```



消息队列

```c
POS_VOID l3IntfInputHook{
	...
    if(0x0806 == uiRet)     
    {
		bRet = sendL3ArpQueue((POS_VOID *)&data);
    }
    ...
}

POS_VOID l3ArpInput(POS_VOID)
{   
    while(1)   
    {
        if(POS_FALSE == receiveL3ArpQueue((POS_VOID*)&data)){
        	work();
        }
    }    
}

gL3ArpTaskId = PosCreateTask(60, "tL3IntfArp", (POSTASKFUNCPTR)l3ArpInput, 0, 32*1024);
```

