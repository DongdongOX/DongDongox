```c
swVlanInit -> bsysLpcVlanInit

/*    vlan初始化     */
rtk_vlan_init(0);
Vlan set tpid inner entry 0 tpid 0x8100
Vlan set tpid inner entry 1 tpid 0x8100
Vlan set tpid inner entry 2 tpid 0x8100
Vlan set tpid inner entry 3 tpid 0x8100
Vlan set tpid outer entry 0 tpid 0x8100
Vlan set tpid outer entry 1 tpid 0x88a8
Vlan set tpid outer entry 2 tpid 0x88a8
Vlan set tpid outer entry 3 tpid 0x88a8
vlan set tpid extra entry 0 tpid 0x8100
vlan set vlan-conversion ingress block-mode 0 conversion 
vlan set vlan-conversion ingress block-mode 1 conversion 
vlan set vlan-conversion ingress block-mode 2 conversion 
vlan set vlan-conversion ingress block-mode 3 conversion 
vlan set vlan-conversion ingress block-mode 4 conversion 
vlan set vlan-conversion ingress block-mode 5 conversion 
vlan set vlan-conversion ingress block-mode 6 conversion 
vlan set vlan-conversion ingress block-mode 7 conversion 

//轮训使用的交换的所有端口
vlan set vlan-conversion ingress port 0 state enable
vlan set vlan-conversion egress port 0 state enable
vlan set ingress port 0 inner keep-tag state enable 
vlan set ingress port 0 outer keep-tag state enable
vlan set ingress port 0 inner tpid 0x1
vlan set ingress port 0 outer tpid 0x0
vlan get pvid inner port 0

vlan创建vlan 10：
vlan create vlan-table vid 10
    
vlan删除:
vlan destory vlan-table vid 10

port type (transparent|hybrid):

配置vlan tag or untag 的 member port(port vlan 10 tagged | untagged):
vlan set vlan-table vid 10 member 1
vlan set vlan-table vid 10 untag-port 1 
vlan set vlan-conversion egress entry 0 key inner tag-status (tagged/untagged)
vlan set vlan-conversion egress entry 0 state enable
vlan set vlan-conversion egress entry 0 key inner vid 10  state enable
vlan set vlan-conversion egress entry 0 key port 1 state enable

配置端口默认vlan（port pvid 10）:
vlan set pvid inner port 1 10 
vlan set pvid outer port 1 10
vlan set vlan-conversion egress entry 0 key inner tag-status (tagged/untagged)
vlan set vlan-conversion egress entry 0 state enable
vlan set vlan-conversion egress entry 0 key inner vid 10 state enable
vlan set vlan-conversion egress entry 0 key port 1 state enable

vlan转换(port modified-vlan 200 2000 7)：
1.  vlan set vlan-conversion ingress entry 0 key port 0 state enable
2.  vlan set vlan-conversion ingress entry 0 key inner vid 200 state enable 
3.  vlan set vlan-conversion ingress entry 0 key inner tag-status all
4.  vlan set vlan-conversion ingress entry 0 key outer tag-status all
5.  vlan set vlan-conversion ingress entry 0 key inner priority 7 state disable
6.  vlan set vlan-conversion ingress entry 0 data status inner non
7.  vlan set vlan-conversion ingress entry 0 data status outer non
8.  vlan set vlan-conversion ingress entry 0 data inner-vid 2000 force
9.  vlan set vlan-conversion ingress entry 0 data inner-priority 7 force
10. vlan set vlan-conversion ingress entry 0 state enable
11. vlan set vlan-conversion egress entry 0 key port 0 state enable
12. vlan set vlan-conversion egress entry 0 key outer tag-status all
13. vlan set vlan-conversion egress entry 0 key inner vid 2000 state enable
14. vlan set vlan-conversion egress entry 0 data inner-vid 200 force
15. vlan set vlan-conversion egress entry 0 state enable
    
删除vlan转换：
vlan set vlan-conversion ingress entry 0 key inner tag-status all   
vlan set vlan-conversion ingress entry 0 key inner tag-status all 
vlan set vlan-conversion ingress entry 0 data status inner none
vlan set vlan-conversion ingress entry 0 data status outer none 
vlan set vlan-conversion ingress entry 0 key inner vid <UINT:vid> state disable
vlan set vlan-conversion ingress entry 0 key outer vid <UINT:vid> state disable
vlan set vlan-conversion ingress entry 0 key inner priority <UINT:priority> state disable
vlan set vlan-conversion ingress entry 0 key outer priority <UINT:priority> state disable
vlan set vlan-conversion ingress entry 0 key port <UINT:port> state disable
vlan set vlan-conversion egress entry 0 state disable

cvlan添加svlan(port nested-vlan 300 2 1)：
vlan set vlan-conversion ingress entry 0 key port 0 state enable
vlan set vlan-conversion ingress entry 0 key inner vid 300 state enable
vlan set vlan-conversion ingress entry 0 key inner tag-status all
vlan set vlan-conversion ingress entry 0 key outer tag-status all
vlan set vlan-conversion ingress entry 0 key outer priority 1 state disable
vlan set vlan-conversion ingress entry 0 data status outer none
vlan set vlan-conversion ingress entry 0 data outer-vid 2 force
vlan set vlan-conversion ingress entry 0 data outer-priority 1 force
vlan set vlan-conversion ingress entry 0 data status outer tag
vlan set vlan-conversion ingress entry 0 data status inner tag
vlan set vlan-conversion ingress entry 0 data outer tpid 0 force
vlan set vlan-conversion ingress entry 0 state enable

删除vlan添加：
vlan set vlan-conversion ingress entry 0 key inner tag-status all   
vlan set vlan-conversion ingress entry 0 key inner tag-status all 
vlan set vlan-conversion ingress entry 0 data status inner none
vlan set vlan-conversion ingress entry 0 data status outer none 
vlan set vlan-conversion ingress entry 0 key inner vid <UINT:vid> state disable
vlan set vlan-conversion ingress entry 0 key outer vid <UINT:vid> state disable
vlan set vlan-conversion ingress entry 0 key inner priority <UINT:priority> state disable
vlan set vlan-conversion ingress entry 0 key outer priority <UINT:priority> state disable
vlan set vlan-conversion ingress entry 0 key port <UINT:port> state disable
```



```c
MAC

MAC初始化：
l2-table set age aging-time auto 300

设置MAC地址学习限制：
l2-table set limit-learning port 0 5000

配置静态MAC地址：
l2-table add mac-ucast 1 00:11:22:33:44:55 port 0 static

删除静态MAC地址：
l2-table del mac-ucast 1 00:11:22:33:44:55
    
MAC地址清除功能:
l2-table set flush flush-entry dynamic-only 
```



```
ETH

ETH初始化：
port set port <> state enable(端口映射后，设置所有的端口使能)
(不同端口可以配置为不同的ability)
port set phy-force port <> ability 1000f
port set serdes-media port <> fiber1g
port set auto-nego port <> state enable
serdes set <sdsId> loopback disable
port set port 24 state enable (outband)
设置outband端口的led初始化状态
port set phy-reg port 24 page 0 register 31 data 0x0d04
port set phy-reg port 24 page 3332 register 16 data 0x6351
port set phy-reg port 24 page 0 register 31 data 0

port admin enable
port set port <> state enable

端口模式设置：
port set phy-force port <> ability 1000f
port set serdes-media port <> fiber1g
port set auto-nego port <> state enable

获取端口的状态：
port dump port <>

获取端口的模式
port dump port <>

获取端口的工作介质(fiber or copper)
port get port <> link-media

获取端口的双工模式
port dump port <>
```



```



```



## VLAN

### 概述

![image-20251115104059815](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251115104059815.png)

系统支持IEEE802.1Q和qinq VLAN(802.1ad)。当收到包时，会基于设置去解析inner tag和outer tag，然后基于inner tag 或outer tag去做vlan学习和转发。当传送数据包时，也可以把包分为untag、inner tag、outer tag或者是double tag

在qinq中，取决于被连接的网络是供应商网路还是服务商网络，每个端口可以被分为两种类型：nni和uni能够识别inner tag和outer tag，uni端口只能识别inner tag.

改交换芯片支持4096个vlans，8个vlan profiles，8个基于端口和协议的vlan entry，1024个基于vlan转换或基于mac vlan table或基于ip子网的ingress vlan entry'和512个egress vlan转换entry。

 通常标准的c-tag的tpid为0x8100,s-tag的tpid为0x88a8,但考虑到许多特殊的环境可能需要定义他们自己的tpid（例如0x9100），也可能需要支持多个ISP，所以系统为用户提供4个inner tpid值和4个outer tpid值的定义。每个端口提供4个bi的i-tpid掩码和4个o-tpid掩码去指示去对比i-tpid和0-tpid设置的哪一个tpid值。

![在这里插入图片描述](file:///C:/Users/15535/AppData/Local/Temp/msohtmlclip1/01/clip_image001.png)

此外，系统支持提供一个全局e-tpid值和eptid_cmp bit，用于为用户定义和选择是否需要比较extra tpid。

一旦数据包匹配上o-tpid/i-tpid/e-tpid，那么这个包就被考虑作为一个outer/inner/exter tag的数据包，没有tag的被视为untag，一个tag被视为single tag，两个tag被视为double tag。

在IEEE802.1Q中，每个端口应该提供一个接收帧类型的属性，这种属性可以是：all franes、only vlan-tagged、only untagged和priority-tagged 

CFI可用于捕获tag/priority-tagged的数据包

![image-20251115111237266](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251115111237266.png)

### vlan分配

在ingress解析中，系统将基于端口vlan、基于端口和协议vlan、ingress vlan translation、基于mac vlan、ip子网和vacl的设置（这些设置也可以进行分配优先级），分别确定ingress i-vid和ingress o-vid。之后，通过对每个端口设置VLAN_PORT_FWD_CTRL，系统将选择ingress i-vid或ingress o-vid作为最后的转发vid，这个vid用于vlan/L2 table查询和转发。但在路由情况下，由于转发的vid在路由前后会有所差异，情况又有所不同。

系统将更具下面的方式来确定接收包的ingress i-vid 和ingress o-vid

![image-20251115143719227](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251115143719227.png)

![image-20251115143730066](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251115143730066.png)

基于端口vlan：每个端口有一个基于端口vlan的配置，被用于控制是否分配OPVID/IPVID的值，这种情况只针对于untagged包或者both tagged and untagged包。IPVID用于为端口提供inner vid，OPVID用于为端口提供outer vid。

基于协议和端口vlan：基于协议和端口vlan提供基于包的帧类型、eth类型和ingress端口来分配vid和优先级的能力。交换芯片提供8个entry对比每个收到的包的帧类型和帧类型值

![image-20251115150506557](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251115150506557.png)

同时，每一个端口支持8中LVAN_PORT_PPB_VLAN_SET设置，以映射8个全局VLAN_PPB_VLAN_SET表。

ingress vlan translation：ivc的功能是在vlan查找前，根据数据包的original vlan信息对vlan信息进行转换。交换芯片支持1024条ingress vlan conversion table（基于mac和基于ip子网vlan）。1024个entry被分为8个blocks（每个128个entry）,交换支持每个block而不是每个entry去表明entry被用于IVC、基于mac、基于ip的vlan。IVC支持多种搜索关键字，包括端口、original inner tag、original outer tag和vlan 范围等。每个ivc entry有一个hit bit，如果包匹配到了ivc规则，hit会被置为1，如果多个entry hit上，最低的index hit被置为1.如果包没有hit到任何ivc规则，将会被丢掉。交换芯片提供一个寄存器来表明查询miss action。交换芯片支持两组ivc vid分为查找表，每组表包含32个专用的范围检查entry。所使用的这两组范围检查表可以根据ingress 端口进行选择。

MAC-based VLAN：系统允许根据数据包的原mac地址分配vid，通过基于mac的vlan 表实现。基于mac的vlan搜索key包含ingress prot、ovid、ivid。此外，还提供一个选项来去控制每个端口基于mac和ip vlan功能的使能。

IP Subnet-based VLAN:IP Subnet-based VLAN和MAC-based VLAN相似，只是主要的查询key从源mac地址变为了源ip地址。

### vlan转发和过滤

系统每个ingress port 和每个 internal-tag-status 提供1 bit去表明包的学习和转发的vid













