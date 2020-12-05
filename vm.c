#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

//求start-end的二进制玛组成的int
int binary_to_int(int binary[],int start,int end){
    int i=0;
    int ans = 0;
    for ( i = start; i < end; i++)
    { 
        ans = ans + pow(2,i-start) * binary[i];
    }
    return ans;
}




int main(void) {
    
    FILE *fp;
    FILE *wfp;
    wfp = fopen("out.txt","w+");
    int len;
    int addresses[1500];//开一个足bai够大的数组。
    int i = 0;
    char address_txt[100] = "addresses.txt";
    if((fp = fopen(address_txt,"r"))== NULL){
        return 0;
    }

    while(fscanf(fp, "%d", &addresses[i]) != EOF) {
        i++;
    }
    fclose(fp);//关闭文件
    //理论上说，为了代码的可阅读性，应该多放几个函数,但是c语言的函数传递，指针调用有点复杂，在这里就不调用了，其实俺觉得这题用别的编程语言也可以
    //以下是对每一个数据的处理。
    //处理的主要核心思路是，得到每一个int数字的二进制字符串，并且，从右往左数，0-7原封不动的给我们的物理地址
    //随后先来先得，将帧玛从0-255依次分配，并且记录下来，这里我觉得可以采用int数组的形式来存取，不需要使用map
    int map[256];

    //严格意义上说，这个就是页表
    char memory[256][256];
    //这个就是我们的物理内存的存放的值
    int j ;
    for(int j = 0; j < 256 ; j++){
        map[j]  = -1;
    }
    //设定这个-1为我们未存取的情况
    int firstFram = 0;
    //当我们查找页表找不到的时候，我们就直接把这个变量分配给他，然后记录到map里面，++就好了
    //对于这个map我觉得，下标用page很方便。
    //也可以把这个数值理解为空闲帧与非空闲帧的分界线。
    for(i = 0; i <1000; i++){
        int temp = addresses[i];
        int count = 0;
        int binary[100];
        int l;
        for(l = 0; l < 100; l++){
            binary[l] = 0;
        }
        while(temp != 0){
            binary[count] = temp & 1;
            //这里相当于temp对000000001按位和，那么只有第一位为1的时候，才会是1，正好是我们想要的结果。
            temp = (temp>>1);
            count++;
            //右移一位，相当于不要了，这里最好注意一下运算符优先级
        }
        //我们得到了二进制的数组，但是是反过来的，不过刚刚好，因为我们运算回去，也是要反过来运算。
        //前8位不用关心，我们直接从第8位开始算，思路上文已经说了
        int page = 0;//页号
        page = binary_to_int(binary,8,16);
        int off_set = 0;
        int m = 0;
        for ( m = 0; m < 8; m++)
        {
            off_set = off_set + pow(2,m) * binary[m];
        }
        
        int frame = 0;
        if(map[page] == -1){
            //发生了缺页错误。我们要去二进制文件进行读取。
            //以下是读取代码
            FILE *file;
            char *buffer;
            int x;
            file = fopen("BACKING_STORE.bin", "rb");
            fseek(file,256*page,0);
            buffer=(char *)malloc(257);
            fread(buffer,1,256,file);
            for ( x = 0; x < 256; x++)
            {
                memory[firstFram][x] = buffer[x];
            }
            free(buffer);
            fclose(file);
            map[page] = firstFram;
            frame = firstFram;
            firstFram++;
        }else{
            //如果有的话，那我们就直接把对应的结果给他就好了，因为是256对256，所以不用担心，如果是10000对256的情况，就要引入页替换了
            frame = map[page];
        }
        //现在我们得到了我们想要的数据，一个是偏移量，一个是对应的帧表数值，结果是要让我们返回二者和起来的数字。
        //frame + offset 和起来的数值，以及对应的value
        char value = memory[frame][off_set];
        int phyAddress[16];
        int y = 0;
        for(y = 0; y < 8; y++){
            phyAddress[y] = binary[y];
        }
        for ( y = 8; y < 16; y++)
        {
            phyAddress[y] = 0;
        }
        int phyCount = 8;
        int temp_f = frame;
        while (temp_f != 0)
        {
            phyAddress[phyCount] = temp_f&1;
            temp_f= (temp_f>>1);
            phyCount++;
        }
        int physicalAddress = binary_to_int(phyAddress,0,16);

        printf("input %d   physcial %d   value:%d \n", addresses[i],physicalAddress,value);    
        char writes[100] = "Virtual address: ";
        char VirtualAddressC[10];
        char PhysicalAddressC[10];
        char ValueS[10];
        sprintf(VirtualAddressC,"%d",addresses[i]);
        sprintf(PhysicalAddressC,"%d",physicalAddress);
        sprintf(ValueS,"%d",value);
        strcat(writes,VirtualAddressC);
        strcat(writes," Physical address: ");
        strcat(writes,PhysicalAddressC);
        strcat(writes," Value: ");
        strcat(writes,ValueS);
        strcat(writes,"\n");
        fputs(writes,wfp);
    }
    return 0;
}










