# Linux以http方式搭建git服务器

linux以http方式搭建git服务器，即clone的方式为git clone http://xxxxxxxx。

用到apache和git-core（支持git的CGI）

1.安装httpd
```
yum install httpd
```

因为很多服务器本身就启动了80端口，所以修改监听端口：

```
vi /etc/httpd/conf/httpd.conf

// 找到 Listen 80修改为：
Listen 8081
```

启动（我这边启动提示：Redirecting to /bin/systemctl start  httpd.service，就用/bin/systemctl start  httpd.service）：
```
service httpd start
```
打开浏览器访问：http://192.168.1.55:8081，如果访问不了，则是因为防火墙，解决：
```
// 添加运行访问8081端口
iptables -A INPUT -p tcp --dport 8081 -j ACCEPT

// 重启保存
service iptables restart

/etc/rc.d/init.d/iptables save

// 如果是阿里云的服务器，保存方法： iptables-save > /etc/sysconfig/iptables
```
2.安装git
```
yum install git
yum install git-core
```

创建仓库：
```
mkdir -p /opt/http_git/test.git
cd /opt/http_git/test.git
git init --bare

// 设置权限
chown -R apache:apache /opt/http_git
```
创建账号：
```
// testuser为账户名 可以随意定义
htpasswd -m -c /etc/httpd/conf.d/git-team.htpasswd testuser

// 修改git-team.htpasswd文件的所有者与所属群组
chown apache:apache /etc/httpd/conf.d/git-team.htpasswd

// 设置git-team.htpasswd文件的访问权限
chmod 640 /etc/httpd/conf.d/git-team.htpasswd
```
设置apache，使其请求转发到git-cgi：
```
vi /etc/httpd/conf/httpd.conf
```
```
// 在最后一行IncludeOptional conf.d/*.conf的上面添加下面内容：
<VirtualHost *:8081>
        ServerName 192.168.1.55
        SetEnv GIT_HTTP_EXPORT_ALL
        SetEnv GIT_PROJECT_ROOT /opt/http_git
        ScriptAlias /git/ /usr/libexec/git-core/git-http-backend/
        <Location />
                AuthType Basic
                AuthName "Git"
                AuthUserFile /etc/httpd/conf.d/git-team.htpasswd
                Require valid-user
        </Location>
</VirtualHost>

///
ServerName是git服务器的域名,这里最后填写你机子的IP
/opt/http_git是代码库存放的文件夹
ScriptAlias是将以/git/开头的访问路径映射至git的CGI程序git-http-backend
AuthUserFile是验证用户帐户的文件
```
保存重启：
```
service httpd restart
```
3.客户端clone命令：
```
git clone http://192.168.1.55:8081/git/test.git
```
会提示输入账号密码，账号为上面设置的testuser 
