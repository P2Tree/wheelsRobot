" ===== 系统路径 ====="
set path=.,~/forlinx/car/robot/**

" ----- ctags设置 -----"
set tags+=~/forlinx/car/robot/tags

" ===== 插件相关 ====="
"
" ===== 编译相关 ====="
nnoremap <silent> <Leader>mm :wa<cr> :make -C ~/forlinx/car/robot/<cr> :cw<cr>
nnoremap <Leader>mc :make clean -C ~/forlinx/car/robot/<cr>
nnoremap <Leader>mi :make install -C ~/forlinx/car/robot/<cr>
