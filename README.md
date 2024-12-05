ESP8266 Pintout
![image](https://github.com/user-attachments/assets/4bb840ff-a412-4a79-ad95-2316863e8b77)

MCP23017 
![image](https://github.com/user-attachments/assets/5db1da11-63ca-42fc-9e34-44f347787df2)
void rain(){
    for(int x=0;x<6;x++){
      for(int y=0;y<6;y++){
          grid[x][y][5]=HIGH;//1 camada toda ligada
          grid[x][y][4]=rand()%2;
          for(int z=4;z>0;z--){
             grid[x][y][z-1]=grid[x][y][z];
          }

      }
    }
}
