import mysql.connector

# 初始化MySQL连接
db = mysql.connector.connect(
    host="your_host",
    user="your_user",
    password="your_password",
    database="your_database"
)

# 初始化缓存字典
query_cache = {}

# 自定义函数来执行查询并使用缓存
def cached_query(query):
    if query in query_cache:
        # 如果查询已经在缓存中，直接返回缓存结果
        return query_cache[query]
    else:
        # 否则，执行查询并将结果存入缓存
        cursor = db.cursor()
        cursor.execute(query)
        result = cursor.fetchall()
        query_cache[query] = result
        return result

# 示例查询
query = "SELECT * FROM your_table WHERE some_condition"
result = cached_query(query)
print(result)

# 关闭数据库连接
db.close()
