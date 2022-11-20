####################################################################
# 停止数据中心后台服务程序的脚本。
####################################################################

killall -9 procctl
killall gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver
killall obtcodetodb obtmindtodbb execsql dminingmysql xmltodb syncupdate yncincrement
killall deletetable migratetable xmltodb_oracle migratetable_oracle deletetable_oracle
killall syncupdate_oracle syncincrement_oracle 

sleep 3

killall -9 gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver
killall -9 obtcodetodb obtmindtodbb execsql dminingmysql xmltodb syncupdate yncincrements
killall -9 deletetable migratetable xmltodb_oracle migratetable_oracle deletetable_oracle
killall -9 syncupdate_oracle syncincrement_oracle
