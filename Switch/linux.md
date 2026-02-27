



```
//挂载：
ls /dev/sd*
//插入u盘，其他的都是多的文件，例如/dev/sdb就是u盘文件，sdb1是u盘的第一个分区
mount -f vfat /dev/sdb1 /mnt/tmp 
umount -t vfat /dev/sdb1
```

