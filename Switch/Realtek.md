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



```
MAC

mac初始化：
l2-table set age aging-time auto 300


```

