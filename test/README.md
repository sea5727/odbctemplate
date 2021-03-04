# 성능 개선

## 대량 Insert 

[Reference](https://www.easysoft.com/products/data_access/odbc_odbc_bridge/performance_white_paper.html#3_1_2)
Bind 파라미터를 사용해라


commit을  끄고, 수행하고 Commit해라
 - 이방법은 Insert 중 다른 작업이 있다면 골치아프므로 이방법은 중간중간 커밋해주도록 할수있다.


SQLSetStmtAttr 를 세팅한다


