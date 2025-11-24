## VLAN

### VLAN流程图

![image-20251119112624655](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251119112624655.png)

### VLAN TPID

能定义4个外层和内层的TPID List,定义1个额外的TPID List.通过使能比特位来控制选择内层的TPID和外层的TPID 是多少以及是否选择多层TPID或者单层TPID。

![image-20251119133531860](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251119133531860.png)

```
//通常设备上默认配置的tpid
RTK.0> vlan get tpid inner   
Inner TPID configuration
 TPID 0: 0x8100
 TPID 1: 0x8100
 TPID 2: 0x8100
 TPID 3: 0x8100
RTK.0> vlan get tpid outer 
Outer TPID configuration
 TPID 0: 0x8100
 TPID 1: 0x88a8
 TPID 2: 0x88a8
 TPID 3: 0x88a8
 RTK.0> vlan get tpid extra 
Extra TPID configuration
 TPID 0: 0x8100
     
//创建VLAN 2     
VLAN create vlan-table vid 2

//配置端口0为VLAN 2 的(member/untag-port)，untag-port会在egress方向剥掉vlan
vlan set vlan-table vid 1 (member/untag-port) 0

//设置tpid（inner/outer都有4个entry）
vlan set tpid (inner/outer) entry 0 tpid 0x8100
//使能比特位来控制选择内层的TPID和外层的TPID
vlan set (ingress/egress) port 0 (inner/outer) tpid 0xF

//设置端口egress方向发出时包的状态
vlan set egress port 0 (inner/outer) status (untag/tag/priority-tag/internal)

//设置接收或发送方向，包的tag会保存
vlan set (ingress/egress) port 0 （inner/outer） keep-tag state enable

RTK.0> vlan dump ingress port 0
Port ingress configuration
Port 0 configuration : 
Inner TPID entry mask   : 0x1
Outer TPID entry mask   : 0x0
Extra tag status        : DISABLE
Keep inner tag format   : ENABLE
Keep outer tag format   : ENABLE
Fwd-VLAN (untagged)     : Inner VLAN
Fwd-VLAN (inner-tagged) : Inner VLAN
Fwd-VLAN (outer-tagged) : Outer VLAN
Fwd-VLAN (double-tag)   : Outer VLAN

RTK.0> vlan dump egress port 0 
Port egress configuration
Port 0 configuration : 
Inner:
TPID Entry Index        : 0
Tag Status              : Internal
Outer:
TPID Entry Index        : 0
Tag Status              : Internal
Extra:
Keep inner tag format   : DISABLE
Keep outer tag format   : DISABLE
Keep inner TPID value   : ENABLE
Keep outer TPID value   : ENABLE
```

### Acceptable Frame Type

每个端口应该提供一个接收帧类型的属性，这种属性可以是：all frame、tag-only、untag-only（include priority tag）。一般都是配置为all frame

```c
RTK.0> vlan get accept-frame-type inner port all 
Accept frame type of ports
Port 0 : accept all frame
Port 1 : accept all frame
Port 2 : accept all frame
Port 3 : accept all frame
Port 8 : accept all frame
Port 9 : accept all frame
Port 10 : accept all frame
Port 11 : accept all frame
Port 16 : accept all frame
Port 17 : accept all frame
Port 18 : accept all frame
Port 19 : accept all frame
Port 24 : accept all frame
Port 26 : accept all frame
Port 27 : accept all frame
RTK.0> vlan get accept-frame-type outer port all 
Accept frame type of outer tag
Port 0 : accept all frame
Port 1 : accept all frame
Port 2 : accept all frame
Port 3 : accept all frame
Port 8 : accept all frame
Port 9 : accept all frame
Port 10 : accept all frame
Port 11 : accept all frame
Port 16 : accept all frame
Port 17 : accept all frame
Port 18 : accept all frame
Port 19 : accept all frame
Port 24 : accept all frame
Port 26 : accept all frame
Port 27 : accept all frame
RTK.0> 
```

### VLAN Decision

根据优先级来决定，VLAN ACL优先级最高，原始tag 内层/外层优先级最低。当一个端口同时配置了VLAN ACL和IVC的话，只有VLAN ACL会生效。

![image-20251119152105464](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251119152105464.png)

#### IVC

IVC包含8个block，128个entries per-block，总共1024个entries。每个block能够被用作IVC或者MAC-based或者IP-subnet-based VLAN。也就是1024个entries被VLAN映射和基于MAC或IP的VLAN分类来分。

##### IVC

```c
//设置block 0的模式为（conversion/mac-based/ip-subnet-based）
vlan set vlan-conversion ingress block-mode 0 （conversion/mac-based/ip-subnet-based）
//使能端口0的vlan-conversion
vlan set vlan-conversion ingress port 0 state enable
//设置端口0的 VLAN 转换规则查找失败，系统对数据包的处理动作为forward。
vlan set vlan-conversion ingress port 0 inner/outer lookup-miss-action forward
//关闭端口 0 接收方向（ingress）VLAN 转换的 “范围检查” 功能 
vlan set vlan-conversion ingress port 0 range-check 0
//启用一个以 “端口 0 入” 为匹配条件的转换规则
vlan set vlan-conversion ingress entry 0 key port 0 state enable
//ivc匹配vlan 500
vlan set vlan-conversion ingress entry 0 key inner vid 50 state enable
//action为把outer-vid强制为100
vlan set vlan-conversion ingress entry 0 data outer-vid 100 force
//action为保留外层 VLAN 标签
vlan set vlan-conversion ingress entry 0 data status outer tag
//action为outer tpid强制为0
vlan set vlan-conversion ingress entry 0 data outer tpid 0 force
//ivc的entry使能
vlan set vlan-conversion ingress entry 0 state enable

RTK.0> vlan get vlan-conversion ingress block-mode 
VLAN conversion block configuration

 Index | Mode 
-------+-----------------------
   0   |Ingress vlan conversion
   1   |Ingress vlan conversion
   2   |Ingress vlan conversion
   3   |Ingress vlan conversion
   4   |Ingress vlan conversion
   5   |Ingress vlan conversion
   6   |Ingress vlan conversion
   7   |Ingress vlan conversion
   
RTK.0> vlan get vlan-conversion ingress port 0 lookup-miss-action
Ingress VLAN conversion Lookup Miss action:
Port 0 : 
        Inner Tag Packet:Forward
        Outer Tag Packet:Forward
        
RTK.0> vlan get vlan-conversion ingress entry 0 
Ingress VLAN conversion entry configuration
 Index : 0
 Enable state : Disable
 ------ KEY SECTION ------
 Accept inner vlan frame type : all packet
 Accept outer vlan frame type : all packet
 Inner VID comparing state : Disable
 Inner VID : 0
 Outer VID comparing state : Disable
 Outer VID : 0
 Range check result : 0x0
 Range check result mask : 0x0
 Inner Priority comparing state : Disable
 Inner Priority : 0
 Outer Priority comparing state : Disable
 Outer Priority : 0
 Port comparing state : Disable
Port Type       : Port
Port Id         : 0
 ------ DATA SECTION ------
 Inner VID operation : none

 New Inner VID : 0
 Outer VID operation : none
 New Outer VID : 0
 Inner Priority operation : none
 New Inner priority : 0
 Outer Priority operation : none
 New Outer priority : 0
 Egress inner tag status: not touch
 Egress outer tag status: not touch
 Inner TPID operation : none
 Inner TPID index : 0
 Outer TPID operation : none
 Outer TPID index : 0
 
 RTK.0>  vlan get vlan-conversion ingress port 0 range-check 
 Port 0 : set 0
```

##### MAC Based VLAN

基于MAC进行分类

```c
//使能端口0的mac-base-vlan
vlan set mac-base-vlan port 0 state enable
//设置vlan-conversion ingress的block 1 为mac-based 
vlan set vlan-conversion ingress block-mode 1 mac-based
//匹配mac地址为11:22:33:44:44:55
vlan set mac-based-vlan entry 128 key mac-address 11:22:33:44:44:55 mac-mask ff:ff:ff:ff:ff:ff
//匹配端口0使能了mac-base-vlan
vlan set mac-based-vlan entry 128 key port 0 state enable
//action为inner vlan置为2，优先级设置为3，tpid设置为1
vlan set mac-based-vlan entry 128 data vlan-type inner vid 2 state enable priority 3 state enable tpid 0x1 state enable
//设置mac-base-vlan的entry 128使能
vlan set mac-base-vlan entry 128 state enable
//设置entry的转发action为forward
vlan set mac-base-vlan entry 128 data fwd-action forward


```

##### IP Subnet Based VLAN

基于IP地址进行分类

```c
//使能端口0的ip-subnet-based-vlan
vlan set ip-subnet-based-vlan port 0 state enable
//设置block为up-subnet-based
vlan set vlan-conversion ingress block-mode 2 ip-subnet-based
//设置key为ip地址为192.168.1.1
vlan set ip-subnet-based-vlan entry 256 key src-ip 192.168.1.1 src-ip-mask 255.255.255.255
//key值为端口8为enable状态
vlan set ip-subnet-based-vlan entry 256 key port 8 state enable
//action为inner vlan设置为20，优先级设置为1
vlan set ip-subnet-based-vlan entry 256 data vlan-type inner vid 20 state enable 
priority 1 state enable tpid 0x0 state disable
//使能entry 256
vlan set ip-subnet-based-vlan entry 256 state enable
//设置转发模式为forward
vlan set ip-subnet-based-vlan entry 256 fwd-action forward

```



#### Protocol-and-Port-based

基于包的帧类型、eth类型和ingress端口来分配vid和优先级的能力。提供8个entry对比每个收到的包的帧类型和帧类型值

```c
//将所有从指定端口（或全局）收到的、以太网帧类型（EtherType）为 0xAABB 的数据包，自动划分到 VLAN 组 0 中
vlan set protocol-vlan group 0 frame-type ethernet frame-value 0xAABB
//在端口 0 上应用 协议 VLAN 组 0 的规则，并配置匹配后的处理方式：将数据包标记为 内层 VLAN 100，优先级为 3，同时启用该配置。
vlan set protocol-vlan port 0 group 0 vlan-type inner vid 100 state enable priority 3 state enable
```



#### Port-based VLAN

Port-based VLAN指的是给端口配置PVID，如果没有高于此优先级的，会给untag报文加上PVID。

```c
//配置端口0的pvid为1，类型为（inner/outer）
vlan set pvid (inner/outer) port 0 1
//配置端口0的pvid-mode为（all/untag-only/umtag-and-priority-tag），用于配置收到那种类型包会在（inner/outer）加上pvid的vlan，一般设置为untag-and-priority-tag
vlan set pvid-mode (inner/outer) port 0 (all/untag-only/untag-and-priority-tag)
```

### VLAN Forward and Filtering

系统支持八个全局 VLAN profiles，其中包含许多 L2 和 L3 控制设置。每个 VLAN 可以选择绑定 VLAN 条目中的 8 个 VLAN 配置文件中的一个，以决定 L2/L3 转发动作

```c
vlan set ingress port 0 fwd-vlan tag-mode inner-tag by inner 
//端口转发时按照inner 还是 outer vlan
RTK.0> vlan dump ingress port 0
Port ingress configuration
Port 0 configuration : 
Inner TPID entry mask   : 0x1
Outer TPID entry mask   : 0x0
Extra tag status        : DISABLE
Keep inner tag format   : ENABLE
Keep outer tag format   : ENABLE
Fwd-VLAN (untagged)     : Inner VLAN
Fwd-VLAN (inner-tagged) : Inner VLAN
Fwd-VLAN (outer-tagged) : Outer VLAN
Fwd-VLAN (double-tag)   : Outer VLAN

//如果vlan没有加入到对应的端口，那么数据包drop
RTK.0> vlan get ingress-filter port all 
Ingress filter configuration
Port 0 : Drop
Port 1 : Drop
Port 2 : Drop
Port 3 : Drop
Port 8 : Drop
Port 9 : Drop
Port 10 : Drop
Port 11 : Drop
Port 16 : Drop
Port 17 : Drop
Port 18 : Drop
Port 19 : Drop
Port 24 : Drop
Port 26 : Drop
Port 27 : Drop

//如果出口过滤使能，则会在对应的出端口那里检查VLAN membership
RTK.0> vlan get egress-filter port all  
Egress filter status of ports
Port 0 : ENABLE
Port 1 : ENABLE
Port 2 : ENABLE
Port 3 : ENABLE
Port 8 : ENABLE
Port 9 : ENABLE
Port 10 : ENABLE
Port 11 : ENABLE
Port 16 : ENABLE
Port 17 : ENABLE
Port 18 : ENABLE
Port 19 : ENABLE
Port 24 : ENABLE
Port 26 : ENABLE
Port 27 : ENABLE
 
//vlan-table
RTK.0> vlan get vlan-table vid 1
Vlan 1 
  Member Ports  : 0-3,8-11,16-19,24,26-27 
  Untag Ports   : 1-3,8-11,16-19,24,26-28 
  Tag Ports     : 0 
  mcast group   : Not support
  Stg           : 0
  UBCAST hkey   : UC_LOOKUP_ON_VID 
  MCAST hkey    : MC_LOOKUP_ON_VID 
  VLAN GroupMask        : 0x0
  Profile index : 0    

//决定l2/l3的转发和learn
RTK.0> vlan get profile entry 0
VLAN profile configuration
 New Src Mac Operation: Asic-AUTO-Learn
 L2 new source mac action: Forward
 IPv4 bridge mode: MAC-based
 IPv6 bridge mode: MAC-based
 L2 bridge look-up-miss action: Forward
 L2 bridge look-up-miss flooding ports: 0-27
 IPv4 bridge look-up-miss action: Forward
 IPv4 bridge look-up-miss flooding ports: 0-27
 IPv6 bridge look-up-miss action: Forward
 IPv6 bridge look-up-miss flooding ports: 0-27
```

### VLAN Remarking

![image-20251119200447890](C:\Users\15535\AppData\Roaming\Typora\typora-user-images\image-20251119200447890.png)

#### EVC

512个egress vlan转换entry

```c
//设置端口0的vlan-conversion使能
vlan set vlan-conversion egress port 0 state enable
//设置端口0的 VLAN 转换规则查找失败，系统对数据包的处理动作为forward。
vlan set vlan-conversion egress port 0 (inner/outer) lookup-miss-action forward
//关闭端口 0 接收方向（egress）VLAN 转换的 “范围检查” 功能 
vlan set vlan-conversion egress port 0 range-check 0
//启用一个以 “端口 0 出” 为匹配条件的转换规则
vlan set vlan-conversion egress range-check 0 0 vid inner 10 20
//使能端口1的entry 0
vlan set vlan-conversion egress entry 0 key port 1 state enable
//设置key值为vlan 100
vlan set vlan-conversion egress entry 0 key (outer/inner) vid 100 state enable
//action为（inner/outer）为untag
vlan set vlan-conversion egress entry 0 data status (inner/outer) untag
//action为inner vlan强制为50
vlan set vlan-conversion egress entry 0 date inner-vid 50 force
//entry 0状态为enable
vlan set vlan-conversion egress entry 0 state enable

```

#### N:1  VLAN Aggregation

N：1 VLAN Aggregation是指，来自 N：1 启用端口且拥有不同客户 VLAN 的所有上行流量都会被转换/聚合到同一 VLAN（这可以通过 IVC 表完成），然后对下游流量进行反向作（可以通过 EVC 表完成）。对于上游未标记的数据包，转发前插入默认 VLAN 标签;如果观察到默认 VLAN，则下游数据包会移除默认 VLAN 标签。

### 设备代码流程

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

//vlan创建vlan 10：
vlan create vlan-table vid 10
    
//vlan删除:
vlan destory vlan-table vid 10

port type (transparent|hybrid):

//配置vlan tag or untag 的 member port(port vlan 555 tagged | untagged):
vlan set vlan-table vid 555 member 0
vlan set vlan-table vid 555 untag-port 0 
vlan set vlan-conversion egress entry 511 key inner tag-status (tagged/untagged)
vlan set vlan-conversion egress entry 511 state enable
vlan set vlan-conversion egress entry 511 key inner vid 1 state enable
vlan set vlan-conversion egress entry 511 key port 0 state enable
    
//配置端口默认vlan（port pvid 10）:
vlan set pvid inner port 0 555 
vlan set pvid outer port 0 555
vlan set vlan-conversion egress entry 512 key inner tag-status (tagged/untagged)
vlan set vlan-conversion egress entry 512 state enable
vlan set vlan-conversion egress entry 512 key inner vid 555 state enable
vlan set vlan-conversion egress entry 512 key port 0 state enable    
 //只设置pvid或pvid和port vlan的vlan值不同时，evc的key配置为该端口下接收vlan为pvid的包，action为不对vlan做任何处理；当pvid和port vlan的vlan值相同时，evc的key配置为该端口下接收vlan为pvid的包，但是action为port vlan配置中包的模式（tag/untag）

//vlan转换(port modified-vlan 200 2000 7)：
vlan set vlan-conversion ingress entry 0 key port 0 state enable
vlan set vlan-conversion ingress entry 0 key inner vid 200 state enable 
vlan set vlan-conversion ingress entry 0 key inner tag-status all
vlan set vlan-conversion ingress entry 0 key outer tag-status all
vlan set vlan-conversion ingress entry 0 key inner priority 7 state disable
vlan set vlan-conversion ingress entry 0 data status inner non
vlan set vlan-conversion ingress entry 0 data status outer non
vlan set vlan-conversion ingress entry 0 data inner-vid 2000 force
vlan set vlan-conversion ingress entry 0 data inner-priority 7 force
vlan set vlan-conversion ingress entry 0 state enable
vlan set vlan-conversion egress entry 0 key port 0 state enable
vlan set vlan-conversion egress entry 0 key outer tag-status all
vlan set vlan-conversion egress entry 0 key inner vid 2000 state enable
vlan set vlan-conversion egress entry 0 data inner-vid 200 force
vlan set vlan-conversion egress entry 0 state enable
    
//删除vlan转换：
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

//cvlan添加svlan(port nested-vlan 300 2 1)：
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

//删除vlan添加：
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

## L2







## L3







## ACL









## Qos









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
l3
l3 interface enable:
l3 set ipuc routing state enable
l3 set ip6uc routing state enable
vlan set profile entry 0 ipuc routing state enable

l3-interface ipv4 A.B.C.D A.B.C.D:



```

