# Projects

## NFS 共享实践

MPI运行需要保证编译后的程序在所有节点都有拷贝，因此需要搭建简单的NFS共享存储服务

### 服务端

服务端安装`nfs-common`, `nfs-kernel-server`

服务端配置`/etc/exports`：

```text
/path/to/server_data  [client_ip|subnet]([rw|ro], sync)
```

> 服务端需要手动创建`/path/to/server_data`

服务端通过`systemctl [op] nfs-kernel-server` 操作NFS服务

### 客户端

客户端安装`nfs-common`

客户端单次挂载

```console
mount server_ip:/path/to/server_data /path/to/client_data
```

客户端开机启动挂载需要修改`/etc/fstab`

```text
server_ip:/path/to/server_data  /path/to/client_data nfs rsize=8192,wsize=8192,timeo=14,intr
```
