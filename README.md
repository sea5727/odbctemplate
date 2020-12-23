# odbctemplate

db library wrapped ODBC

example 
1.  SELECT
-  auto result = conn.preparedExecute("select id, test, name from tuto where name=?;", "searchname")
            .fetch<tuto>([](odbctemplate::OdbcFetcher::FetchHelper helper){
                tuto result;
                result.id = helper.getLong();
                result.test = helper.getString();
                result.name = helper.getString();
                return result;
            });
    for(auto & tuto : result2){
        tuto.print();

2. INSERT/UPDATE/DELETE

