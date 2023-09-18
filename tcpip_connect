import socket

# MySQL服务器地址和端口
mysql_host = '127.0.0.1'
mysql_port = 3306

# 创建一个TCP套接字
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # 连接到MySQL服务器
    client_socket.connect((mysql_host, mysql_port))

    # MySQL登录信息
    username = 'root'
    password = '000000'
    database = 'test'

    # 构建MySQL登录请求
    login_request = (
        b'\x0a' +  # 协议版本
        username.encode('utf-8') + b'\x00' +
        b'\x14' +  # 加密插件数据长度
        b'\x00' * 21 +  # 填充
        password.encode('utf-8') + b'\x00' +
        database.encode('utf-8') + b'\x00'
    )

    # 发送登录请求
    client_socket.sendall(login_request)

    # 接收并打印服务器的响应
    response = client_socket.recv(1024)
    # 解析响应
    version_bytes = response[4:8]
    plugin_name_bytes = response[-len(b'caching_sha2_password'):]
    version = version_bytes.decode('utf-8')
    plugin_name = plugin_name_bytes.decode('utf-8')

    print(f"版本号: {version}")
    print(f"加密插件名称: {plugin_name}")

except Exception as e:
    print(f"发生错误: {e}")

finally:
    # 关闭套接字
    client_socket.close()
# 提取头部信息
header = response[:4]
response_length = int.from_bytes(header, byteorder='little')

# 提取数据包
data_packet = response[4:]

# 打印响应长度和数据包内容
print(f"响应长度: {response_length}")
print(f"数据包内容: {data_packet}")
