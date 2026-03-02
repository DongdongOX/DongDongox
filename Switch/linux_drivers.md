### 驱动模型的基石

#### kobject

kobject是设备模型中用于表示设备的基本数据结构。每一个设备都有一个对应的kobject。 struct device和struct device_driver结构体中包含kobject。

```c
struct kobject {                                                                                                        
    const char      *name;								//对应sysfs下的一个目录
    struct list_head    entry;							//嵌入kobj的双向链表
    struct kobject      *parent;						//指向kobj父对象的指针
    struct kset     *kset;								//表示kobj所属的集合
    const struct kobj_type  *ktype;						//表示kobj所属对象的类型
    struct kernfs_node  *sd;							//该kobject在sysfs中的表示
    struct kref     kref;								//kobj的引用计数，当引用计数为0，调用release函数

    unsigned int state_initialized:1;					//初始化时置，表示对象已被初始化
    unsigned int state_in_sysfs:1;						//kobj在sysfs中的状态，是否已在sysfs中呈现，以便在自动注销时从sysfs中移除，对应目录被创建置1，否则为0
    unsigned int state_add_uevent_sent:1;	//添加设备的uevent事件是否发送标志，添加设备时候会向用户空间发送uevent事件，请求添加设备。记录是否已经向用户空间发送ADD uevent
    unsigned int state_remove_uevent_sent:1;//删除设备的uevent事件是否发送标志，删除设备时会向用户空间发送uevent事件，请求卸载设备。
    unsigned int uevent_suppress:1; //如果字段为1，则表示忽略所有上报的uevent事件
};
```

```c
//主要的api
void kobject_init(struct kobject *kobj, struct kobj_type *ktype);	//初始化
int kobject_add(struct kobject *kobj, struct kobject*parent,const char *fmt, …);	//初始化后的kobject添加到内核
int kobject_init_and_add(struct kobject*kobj, struct kobj_type *ktype, struct kobject*parent, const char *fmt, …); //上两个接口组合
extern void kobject_del(struct kobject *kobj); //删除
kobject_set_name_vargs //设置 kobject 的 name
kobject_set_name 	//对上个函数的封装, 用于设置 kobject 的 name
kobject_rename 		//修改 kobject 的名字
kobject_get 		//调用 kref_get(&kobj->kref), 来增加 kobject 的引用计数
kobject_put 		//调用 kref_put(&kobj->kref, kobject_release)来减少 kobject 的引用计数
```

#### kset

kset是一组相关设备对象（kobject）的集合。允许将相关的设备对象组织在一起，形成层次结构，方便设备的管理和查找。bus_type结构体中包含kset。

```c
struct kset {
	struct list_head list;						//一个双向链表
	spinlock_t list_lock;						//list操作的自旋锁
	struct kobject kobj;						//kset本身也是一个kobject
	const struct kset_uevent_ops *uevent_ops;	//kset的uevent函数
} __randomize_layout;

struct kset_uevent_ops {
	int (* const filter)(const struct kobject *kobj);
	const char *(* const name)(const struct kobject *kobj);	//该接口可以返回 kset 的名称. 
	int (* const uevent)(const struct kobject *kobj, struct kobj_uevent_env *env);
};

```

```c
//相关api
extern void kset_init(struct kset *kset);	//
extern int __must_check kset_register(struct kset*kset);	//先调用 kset_init,然后调用 kobject_add_internal 将其 kobject 添加到kernel,然后发送一个 KOBJ_ADD uevent 事件
extern void kset_unregister(struct kset *kset)		//
extern struct kset * __must_check kset_create_and_add(const char *name, const struct kset_uevent_ops *u, struct kobject *parent_kobj)
```



#### ktype

ktype定义了一组操作函数，用于处理特定类型的设备对象。

```c
struct kobj_type {                                                                                                      
    void (*release)(struct kobject *kobj);			//kobj的释放接口
    const struct sysfs_ops *sysfs_ops;				//读写属性文件时的调用函数
    const struct attribute_group **default_groups;	//kobj的默认属性
    const struct kobj_ns_type_operations *(*child_ns_type)(const struct kobject *kobj);
    const void *(*namespace)(const struct kobject *kobj);
    void (*get_ownership)(const struct kobject *kobj, kuid_t *uid, kgid_t *gid);
};

struct sysfs_ops {
	ssize_t	(*show)(struct kobject *, struct attribute *, char *);
	ssize_t	(*store)(struct kobject *, struct attribute *, const char *, size_t);
};
```

#### Attribute

一个感性的认识是: Attribute 就是/sys/某个目录下的一个文件, 这个文件叫属性文件. Attribute 与 Kobject 的关系也很明显, 文件是存在于某个目录下的, 所以 Attribute 也是跟 Kobject 挂钩的. 我们一般说某个 Kobject 具有某些属性. Attribute 有两个主要参数: name: 名字, /sys/下的文件名, 就是这个 name mode: 权限, 该文件的读写权限。

VFS -> file_operations -> sysfs(kernfs_ops) -> (kobject->ktype) ->(ktype->sysfs_ops).

内 核 不 能 直 接 访 问 用 户 空 间 的 buffer, 需 要 用copy_to_user/copy_from_user  

```c
// 头文件：include/linux/sysfs.h
struct attribute {
    const char      *name;   // sysfs 中显示的文件名（如 "temperature"）
    umode_t         mode;    // 文件权限（如 0644：用户可读可写，组/其他只读）
#ifdef CONFIG_DEBUG_LOCK_ALLOC
    bool            ignore_lockdep:1;
    struct lock_class_key *key;
    struct lock_class_key skey;
#endif
};
```

```c
sysfs_create_file(struct kobject *kobj,const struct attribute *attr);	//在某个 kobject 下创建 1 个属性文件
sysfs_remove_file(struct kobject *kobj,const struct attribute *attr);	//删除 kobject 下的某个属性文件
sysfs_create_group(struct kobject *kobj,const struct attribute_group *grp);	//在 kobject 下创建多个属性文件, attribute_group 相当于一个数组, 里面可以存放多个
sysfs_remove_group(struct kobject *kobj, const struct attribute_group *grp);		//删除 kobject 下的某个属性组
sysfs_create_groups(struct kobject *kobj, const struct attribute_group **groups);	//创建多个属性组
sysfs_remove_groups(struct kobject *kobj,const struct attribute_group **groups);	//删除多个属性组
```

#### 代码：内核模块手动加载到内核

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *example_kobj;

static int kobj_attr_value = 0;

/*the attribute for the kobject*/
static struct attribute kobj_attr = {
	.name = "kobj_attr",
	.mode = VERIFY_OCTAL_PERMISSIONS(0664),
};

static void ktype_release(struct kobject *kobj)
{
	printk(KERN_ALERT "release kobject (%p)\n", kobj);
	kfree(kobj);
}

static ssize_t kobject_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kobj_attr_value);
	
	return ret;
}

static ssize_t kobject_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kobj_attr_value);
	
	printk(KERN_ALERT "attribute value from user %s\n", buf);

	ret = count;
	
	return ret;
}

const struct sysfs_ops kobject_sysfs_ops = {
	.show	= kobject_attr_show,
	.store	= kobject_attr_store,
};

/*your own ktype for the kobject*/
static struct kobj_type ktype = {
	.release = ktype_release,
	.sysfs_ops = &kobject_sysfs_ops,
};

static int __init example_init(void)
{
	int retval;
	
	/*first: allocate a kobject memory*/
	example_kobj = kzalloc(sizeof(*example_kobj), GFP_KERNEL);
	if (!example_kobj)
		return -ENOMEM;
	
	/*second: define your own ktype, and init the kobject*/
	kobject_init(example_kobj, &ktype);
	
	/*third: add the kobject to kernel*/
	retval = kobject_add(example_kobj, NULL, "%s", "example_kobj");
	if (retval) {
		printk(KERN_WARNING "%s: kobject_add error: %d\n",
		       __func__, retval);
		goto kobject_add_error;
	}
	
	/*forth: create the attribute file associated with this kobject */
	retval = sysfs_create_file(example_kobj, &kobj_attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file error: %d\n",
		       __func__, retval);
		goto create_attribute_error;
	}
	
	return 0;

kobject_add_error:
create_attribute_error:
	kobject_put(example_kobj);
	example_kobj = NULL;
	return retval;
}

static void example_exit(void)
{
	kobject_put(example_kobj);
	example_kobj = NULL;
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of manual create kobject and attribute");
```

#### 代码：内核模块自动加载到内核

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *example_kobj;

static int kobj_attr_value = 0;

static ssize_t kobject_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kobj_attr_value);
	
	return ret;
}

static ssize_t kobject_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kobj_attr_value);
	
	printk(KERN_ALERT "attribute value from user %s\n", buf);

	ret = count;
	
	return ret;
}

/*the attribute for the kobject*/
static struct kobj_attribute kobj_attr =
	__ATTR(kobj_attr, 0664, kobject_attr_show, kobject_attr_store);

static int __init example_init(void)
{
	int retval;
	
	/*first: create a kobject and add it to kernel*/
	example_kobj = kobject_create_and_add("example_kobj", NULL);
	if (!example_kobj)
		return -ENOMEM;
	
	/*second: create the attribute file associated with this kobject */
	retval = sysfs_create_file(example_kobj, &kobj_attr.attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file error: %d\n",
		       __func__, retval);
		goto create_attribute_error;
	}
	
	return 0;

create_attribute_error:
	kobject_put(example_kobj);
	example_kobj = NULL;
	return retval;
}

static void example_exit(void)
{
	kobject_put(example_kobj);
	example_kobj = NULL;
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of auto create kobject and attribute");
```

#### 代码：在/sys/下建 1 个父目录, 该父目录下会有 1 个属性文件, 1 个子目录, 子目录下会有 1 个属性文件. 可以读写这 2 个属性文件  

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

static struct kset    *example_kset;
static struct kobject *example_kobj;

static int kset_attr_value = 0;
static int kobj_attr_value = 0;

/*
 * functions for kset
 */

/*the attribute for the kset*/
static struct attribute kset_attr = {
	.name = "kset_attr",
	.mode = VERIFY_OCTAL_PERMISSIONS(0664),
};

static void kset_self_release(struct kobject *kobj)
{
	struct kset *kset = container_of(kobj, struct kset, kobj);
	printk(KERN_ALERT "release kset (%p)\n", kset);
	kfree(kset);
}

static ssize_t kset_kobj_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kset_attr_value);
	
	return ret;
}

static ssize_t kset_kobj_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kset_attr_value);
	
	printk(KERN_ALERT "attribute value from user for kset %s\n", buf);

	ret = count;
	
	return ret;
}

const struct sysfs_ops kset_kobj_sysfs_ops = {
	.show	= kset_kobj_attr_show,
	.store	= kset_kobj_attr_store,
};

/*your own ktype for the kset's kobject*/
static struct kobj_type kset_self_ktype = {
	.release = kset_self_release,
	.sysfs_ops = &kset_kobj_sysfs_ops,
};

/*
 * functions for kobject
 */
 
static ssize_t kobject_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kobj_attr_value);
	
	return ret;
}

static ssize_t kobject_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kobj_attr_value);
	
	printk(KERN_ALERT "attribute value from user for kobject %s\n", buf);

	ret = count;
	
	return ret;
}

/*the attribute for the kobject*/
static struct kobj_attribute kobj_attr =
	__ATTR(kobj_attr, 0664, kobject_attr_show, kobject_attr_store);

static void ktype_release(struct kobject *kobj)
{
	printk(KERN_ALERT "release kobject (%p)\n", kobj);
	kfree(kobj);
}

/*your own ktype for the kobject*/
static struct kobj_type kobject_ktype = {
	.release	= ktype_release,
	/*Note: 
	 * Here we don't define the ops but use kobj_sysfs_ops which is defined in kobject.c
	 * because we have done it in manual_kobject_attribute, don't want to do it again
	 */
	.sysfs_ops	= &kobj_sysfs_ops,
};

static int __init example_init(void)
{
	int retval;
	
	/*first: allocate a kset memory and prepare the kobj.ktype for this kset*/
	example_kset = kzalloc(sizeof(*example_kset), GFP_KERNEL);
	if(!example_kset)
		return -ENOMEM;
	retval = kobject_set_name(&example_kset->kobj, "%s", "example_kset");
	if (retval) {
		kfree(example_kset);
		return retval;
	}
	example_kset->uevent_ops = NULL;
	example_kset->kobj.parent = NULL;
	example_kset->kobj.ktype = &kset_self_ktype;
	example_kset->kobj.kset = NULL;
	
	/*second: register the kset*/
	retval = kset_register(example_kset);
	if (retval) {
		kfree(example_kset);
		return retval;
	}
	
	/*third: create the attribute file associated with this kset*/
	retval = sysfs_create_file(&example_kset->kobj, &kset_attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file for kset error: %d\n",
		       __func__, retval);
		goto create_kset_attribute_error;
	}
	
	/*4th: allocate a kobject memory*/
	example_kobj = kzalloc(sizeof(*example_kobj), GFP_KERNEL);
	if (!example_kobj) {
		retval = -ENOMEM;
		goto allocate_kobject_error;
	}
	
	/*5th: define your own ktype, and init the kobject*/
	kobject_init(example_kobj, &kobject_ktype);
	
	/*6th: set the kobject's kset*/
	example_kobj->kset = example_kset;
	
	/*7th: add the kobject to kernel*/
	retval = kobject_add(example_kobj, NULL, "%s", "example_kobj");
	if (retval) {
		printk(KERN_WARNING "%s: kobject_add error: %d\n",
		       __func__, retval);
		goto kobject_add_error;
	}
	
	/*8th: create the attribute file associated with this kobject */
	retval = sysfs_create_file(example_kobj, &kobj_attr.attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file error: %d\n",
		       __func__, retval);
		goto create_attribute_error;
	}
	
	return 0;

create_attribute_error:
kobject_add_error:
	kobject_put(example_kobj);
allocate_kobject_error:
	example_kobj = NULL;
create_kset_attribute_error:
	kset_unregister(example_kset);
	example_kset = NULL;
	return retval;
}

static void example_exit(void)
{
	kobject_put(example_kobj);
	example_kobj = NULL;
	kset_unregister(example_kset);
	example_kset = NULL;
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of manual create kobject and attribute");
```

#### uevent

Uevent 的机制是比较简单的, 设备模型中任何设备有事件需要上报时, 会触发Uevent 提供的接口.Uevent 模块准备好上报事件的格式后, 可以通过两个途径把事件上报到用户空间：一种是通过 kmod 模块, 直接调用用户空间的可执行文件(简称 uevent helper); 另一种是通过 netlink 通信机制, 将事件从内核空间传递给用户空间

#### sysfs

sysfs 就是一种虚拟的文件系统, 它的代码路径在 fs/sysfs. 它的初始函数为 sysfs_init, 在该函数中, 会调用 register_filesystem, 向 VFS 注册虚拟文件系统.
说它基于 RAM 的意思是, sysfs 是构建在 RAM 上的, 用户空间所有对 sysfs 的增/删/修改操作, 重启之后都没了. 比如你在 sysfs 下创建了一个文件, 重启之后, 这个文件就没了。



### BUS

总线是处理器与一个或者多个设备之间的通道, 为了方便设备模型的实现, 内核规定, 系统中的每个设备都要连接在一个 Bus 上. 这个 Bus 可以是一个内部 Bus、虚拟 Bus 或者 Platform Bus.

Bus 在内核中的作用主要体现为 :

➢ Bus 下面挂接 devices 和 drivers 

➢ Bus 定义 device 和 driver 的匹配规则 

➢ Bus 可以定义下属 devices 和 drivers 的默认属性

```c
// 总线类型核心结构体
struct bus_type {
    const char      *name;          // 总线名称（如"usb"、"pci"、"platform"）
    const char      *dev_name;      // 总线下设备的默认名称前缀
    struct device   *dev_root;      // 该总线的根设备（sysfs中对应目录）
    struct device_attribute *dev_attrs; // 总线下所有设备的默认属性

    // 1. 设备-驱动匹配核心函数（最关键）
    int (*match)(struct device *dev, struct device_driver *drv);
    // 2. 驱动加载时的回调（驱动绑定到总线时执行）
    int (*probe)(struct device *dev);
    // 3. 驱动卸载时的回调（驱动和设备解绑时执行）
    int (*remove)(struct device *dev);
    // 4. 设备断电时的回调
    void (*shutdown)(struct device *dev);

    // 电源管理相关（可选）
    int (*suspend)(struct device *dev, pm_message_t state);
    int (*resume)(struct device *dev);

    // 总线的kobject（关联到内核设备模型）
    struct kobject *p;
    // 总线的属性链表（bus_attribute）
    struct subsys_private *p;  // 内部私有数据，管理总线的设备/驱动列表
    struct lock_class_key lock_key;

    // sysfs相关操作（总线属性的读写规则）
    const struct attribute_group **bus_groups;    // 总线自身的属性组
    const struct attribute_group **dev_groups;    // 总线下设备的属性组
    const struct attribute_group **drv_groups;    // 总线下驱动的属性组

    // 热插拔事件相关
    int (*uevent)(struct device *dev, struct kobj_uevent_env *env);

    // 其他辅助成员（省略部分小众成员）
    unsigned int drivers_autoprobe:1;
};
```

内核用 subsys_private 这样一个数据结构抽象了各种 sub-system 中的公共内容.

```c
// 总线子系统私有数据核心结构体
struct subsys_private {
    // 1. 总线的核心链表（最关键）
    struct klist klist_devices;  // 该总线下所有设备的链表（struct device）
    struct klist klist_drivers;  // 该总线下所有驱动的链表（struct device_driver）
    
    // 2. 同步锁：保护链表的并发访问（防止多线程同时修改链表）
    struct rw_semaphore rwsem;   // 读写信号量：读共享、写独占
    
    // 3. 内核对象相关（融入设备模型）
    struct kset subsys;          // 总线对应的kset（kobject集合）
    struct kset *devices_kset;   // 总线下设备的kset
    struct kset *drivers_kset;   // 总线下驱动的kset
    
    // 4. 指向所属的bus_type（反向关联）
    struct bus_type *bus;        // 关联到对应的总线类型
    
    // 5. sysfs目录相关（文件系统挂载点）
    struct kobject *devices_kobj;// 对应/sys/bus/xxx/devices/目录的kobject
    struct kobject *drivers_kobj;// 对应/sys/bus/xxx/drivers/目录的kobject
    
    // 6. 热插拔（uevent）相关
    struct blocking_notifier_head bus_notifier;
    unsigned int drivers_autoprobe:1;  // 是否自动匹配设备和驱动
    
    // 7. 其他辅助成员（省略小众成员）
    struct klist_node node;
    struct module *owner;
};
```

主要的api

```c
//注册,创建了/sys/bus/xxx,并且在xxx下创建了devices、drivers、drivers_autoprobe、drivers_probe、uevent
bus_register(struct bus_type *bus);	
bus_unregister(struct bus_type *bus);	//解注册
bus_notifier_register(struct bus_type *bus, struct notifier_block *nb);		//注册总线事件通知器
bus_notifier_unregister(struct bus_type *bus, struct notifier_block *nb);	//注销总线事件通知器
bus_create_file(struct bus_type *bus, struct bus_attribute *attr);	//为总线创建 sysfs 属性文件
bus_remove_file(struct bus_type *bus, struct bus_attribute *attr);	//删除总线的 sysfs 属性文件
//将该 device 在 sysfs 中的目录, 链接到该 bus 的 devices 目录下
bus_add_device(struct device *dev);		
bus_add_driver(struct device_driver *drv);	//将驱动添加到总线的驱动链表（subsys_private->klist_drivers）
int bus_probe_device(struct device *dev);
```

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */
#include <linux/device.h>   /*bus, device, device_driver*/

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

/*
 * Bus related code
 ******************************************************************************
 */
static int bus_attr_value = 0;

ssize_t bus_attr_store(struct bus_type *bus,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &bus_attr_value);
	
	printk(KERN_ALERT "bus attribute value from user %s\n", buf);
	
	ret = count;
	
	return ret;
}

ssize_t bus_attr_show(struct bus_type *bus, char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", bus_attr_value);
	
	return ret;
}

/*the attribute for the bus*/
static BUS_ATTR(embus_attr, 0664, bus_attr_show, bus_attr_store);

static struct attribute *attrs[] = {
	&bus_attr_embus_attr.attr,
	NULL,
};

static struct attribute_group bus_attr_group = {
	.attrs = attrs,
};

const struct attribute_group *bus_attr_groups[] = {
	&bus_attr_group,
	NULL,
};

struct bus_type embest_bus = {
	.name = "embestBus",
	.bus_groups = bus_attr_groups,
	//we don't define a match function, so devices and drivers under this bus will
	//always match.
};

/*
 * Device related code
 ******************************************************************************
 */
static struct device embest_device = {
	.init_name = "embestDev",
	.bus = &embest_bus,
	//no release function, when do device_unregister, kernel will give a WARNING
	//but it don't affect
};

 /*
 * Driver related code
 ******************************************************************************
 */
 
static int embest_device_driver_probe(struct device *dev)
{
	printk(KERN_ALERT "embest_device_driver_probe, device name %s \n", dev_name(dev));
	
	return 0;
}

static int embest_device_driver_remove(struct device *dev)
{
	printk(KERN_ALERT "embest_device_driver_remove, device name %s \n", dev_name(dev));
	
	return 0;
}
static struct device_driver embest_driver = {
	.name = "embestDev",
	.owner		= THIS_MODULE,
	.bus  = &embest_bus,
	.probe = embest_device_driver_probe,
	.remove = embest_device_driver_remove,
};
 
static int __init example_init(void)
{
	int error;
	
	error =  bus_register(&embest_bus);
	if (error) {
		printk(KERN_ALERT "embest bus register failed \n");
		goto bus_register_fail;
	}
	
	error = device_register(&embest_device);
	if (error) {
		printk(KERN_ALERT "embest device register failed \n");
		goto device_register_fail;
	}
	
	error = driver_register(&embest_driver);
	if (error) {
		printk(KERN_ALERT "embest driver register failed \n");
		goto driver_register_fail;
	}

	return 0;

driver_register_fail:
	device_unregister(&embest_device);
device_register_fail:
	bus_unregister(&embest_bus);
bus_register_fail:
	return error;
}

static void example_exit(void)
{
	driver_unregister(&embest_driver);
	device_unregister(&embest_device);
	bus_unregister(&embest_bus);
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of bus, device, device_driver");
```

### CLASS

类是一个设备的高层视图, 它抽象出了设备的共性.几乎所有的类都显示在/sys/class 目录中. 例如输入设备/sys/class/input; 串行设备/sys/class/tty.一个例外是块设备, 出于历史原因, block class 是在/sys/block.所谓 block class 就是 name为”block”的 class.

类成员通常被上层代码所控制, 而不需要来自驱动程序的明确支持. 极少的情况下, 驱动程序需要直接处理类

```c
// 设备类核心结构体
struct class {
    const char              *name;          // 类名称（如"input"、"block"、"net"）
    struct module           *owner;         // 所属模块

    // 1. 设备节点（/dev/）创建/销毁回调
    struct device *(*devnode)(struct device *dev, umode_t *mode);
    void (*dev_release)(struct device *dev); // 类下设备的释放函数

    // 2. 类的属性操作（sysfs）
    const struct attribute_group **class_groups; // 类自身的属性组
    const struct attribute_group **dev_groups;   // 类下设备的属性组

    // 3. 热插拔（uevent）相关
    int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
    const char *(*devnode)(struct device *dev, umode_t *mode);

    // 4. 电源管理相关（可选）
    int (*suspend)(struct device *dev, pm_message_t state);
    int (*resume)(struct device *dev);

    // 5. 内核设备模型关联（核心）
    struct subsys_private *p;  // 类的私有数据（和bus_type的p类似）
    struct lock_class_key lock_key;

    // 6. 设备节点创建的辅助参数
    bool suppress_auto_sysfs:1;
    bool suppress_attrs:1;
    bool ns_capable:1;
    bool devtmpfs:1;           // 是否自动在/dev/创建设备节点
    enum class_dev_iter_type dev_iter_type;
};
```

```c
// 设备类接口核心结构体
struct class_interface {
    // 1. 关联的设备类（必须赋值，指定监听哪个类）
    struct class *class;

    // 2. 核心回调函数（监听事件）
    // 当接口注册到类、且类中已有设备时，对每个设备执行此回调
    int (*add_dev)(struct device *dev, struct class_interface *intf);
    // 当类中移除设备时，执行此回调
    void (*remove_dev)(struct device *dev, struct class_interface *intf);
    // （可选）接口自身注册/注销时的回调（部分内核版本支持）
    int (*add)(struct class_interface *intf);
    void (*remove)(struct class_interface *intf);

    // 3. 内核内部管理链表（无需手动操作）
    struct list_head node;
};
```

```c
//主要api
class_create(struct module *owner, const char *name);
class_destroy(struct class *cls);
class_register(struct class *cls);
class_unregister(struct class *cls);
class_create_file(struct class *cls, const struct class_attribute *attr);
class_remove_file(struct class *cls, const struct class_attribute *attr);
class_add_group(struct class *cls, const struct attribute_group *grp);
class_remove_group(struct class *cls, const struct attribute_group *grp);
class_interface_register(struct class_interface *intf);
class_interface_unregister(struct class_interface *intf);

```

#### 创建一个新的基本的 class, 所谓基本的意思, 就是仿造__class_create, 只提供一些必要的功能

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */
#include <linux/device.h>   /*device, class*/
#include <linux/err.h>		/*IS_ERR, ERR_PTR*/

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

/*
 * Class related code
 ******************************************************************************
 */
static struct class *embest_class;

/*
 * Device related code
 ******************************************************************************
 */
static struct device *embest_device;

static int __init example_init(void)
{
	embest_class = class_create(THIS_MODULE, "embestClass");
	if (IS_ERR(embest_class)) {
		printk(KERN_ALERT "create embest class error\n");
		return -ENODEV;
	}
	
	embest_device = device_create(embest_class, NULL, 0, NULL, "embestDev");
	if (IS_ERR(embest_device)) {
		printk(KERN_ALERT "create embest device error\n");
		return -ENODEV;
	}
	
	return 0;

}

static void example_exit(void)
{
	device_destroy(embest_class, 0);
	class_destroy(embest_class);
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of class, device");
```

