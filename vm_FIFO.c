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


//对LRU数据结构的实现
#define Nothingness -1

struct node{
    int key;
    int value;
    struct node* prev;
    struct node* next;
};//双向链表

struct hash{
    struct node* unused;//数据的未使用时长
    struct hash* next;//拉链法解决哈希冲突
};//哈希表结构

typedef struct {    
    int size;//当前缓存大小
    int capacity;//缓存容量
    struct hash* table;//哈希表
    //维护一个双向链表用于记录 数据的未使用时长
    struct node* head;//后继 指向 最近使用的数据
    struct node* tail;//前驱 指向 最久未使用的数据    
} LRUCache;

struct hash* HashMap(struct hash* table, int key, int capacity)
{//哈希地址
    int addr = key % capacity;//求余数
    return &table[addr];
}

void HeadInsertion(struct node* head, struct node* cur)
{//双链表头插法
    if (cur->prev == NULL && cur->next == NULL)
    {// cur 不在链表中        
        cur->prev = head;
        cur->next = head->next;
        head->next->prev = cur;
        head->next = cur;
    }
    else
    {// cur 在链表中
        struct node* fisrt = head->next;//链表的第一个数据结点
        if ( fisrt != cur)
        {//cur 是否已在第一个
            cur->prev->next = cur->next;//改变前驱结点指向
            cur->next->prev = cur->prev;//改变后继结点指向
            cur->next = fisrt;//插入到第一个结点位置
            cur->prev = head;
            head->next = cur;
            fisrt->prev = cur;
        }
    }
}

LRUCache* lRUCacheCreate(int capacity) {
    /*if (capacity <= 0)
    {//传参检查
        return NULL;
    }*/
    LRUCache* obj = (LRUCache*)malloc(sizeof(LRUCache));
    obj->table = (struct hash*)malloc(capacity * sizeof(struct hash));
    memset(obj->table, 0, capacity * sizeof(struct hash));
    obj->head = (struct node*)malloc(sizeof(struct node));
    obj->tail = (struct node*)malloc(sizeof(struct node));
    //创建头、尾结点并初始化
    obj->head->prev = NULL;
    obj->head->next = obj->tail;
    obj->tail->prev = obj->head;
    obj->tail->next = NULL;
    //初始化缓存 大小 和 容量 
    obj->size = 0;
    obj->capacity = capacity;
    return obj;
}

int lRUCacheGet(LRUCache* obj, int key) {
    struct hash* addr = HashMap(obj->table, key, obj->capacity);//取得哈希地址
    addr = addr->next;//跳过头结点
    if (addr == NULL){
        return Nothingness;
    }
    while ( addr->next != NULL && addr->unused->key != key)
    {//寻找密钥是否存在
        addr = addr->next;
    }
    if (addr->unused->key == key)
    {//查找成功
        HeadInsertion(obj->head, addr->unused);//更新至表头
        return addr->unused->value;
    }
    return Nothingness;
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    struct hash* addr = HashMap(obj->table, key, obj->capacity);//取得哈希地址
    if (lRUCacheGet(obj, key) == Nothingness)
    {//密钥不存在
        if (obj->size >= obj->capacity)
        {//缓存容量达到上限
            struct node* last = obj->tail->prev;//最后一个数据结点
            struct hash* remove = HashMap(obj->table, last->key, obj->capacity);//舍弃结点的哈希地址
            struct hash* ptr = remove;
            remove = remove->next;//跳过头结点
            while (remove->unused->key != last->key)
            {//找到最久未使用的结点
                ptr = remove;
                remove = remove->next;
            }
            ptr->next = remove->next;//在 table[last->key % capacity] 链表中删除结点
            remove->next = NULL;
            remove->unused = NULL;//解除映射
            free(remove);//回收资源
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->next = addr->next;//连接到 table[key % capacity] 的链表中
            addr->next = new_node;
            new_node->unused = last;//最大化利用双链表中的结点，对其重映射(节约空间)
            last->key = key;//重新赋值
            last->value = value;
            HeadInsertion(obj->head, last);//更新最近使用的数据
        }
        else
        {//缓存未达上限
            //创建(密钥\数据)结点,并建立映射
            struct hash* new_node = (struct hash*)malloc(sizeof(struct hash));
            new_node->unused = (struct node*)malloc(sizeof(struct node));
            new_node->next = addr->next;//连接到 table[key % capacity] 的链表中
            addr->next = new_node;
            new_node->unused->prev = NULL;//标记该结点是新创建的,不在双向链表中
            new_node->unused->next = NULL;
            new_node->unused->key = key;//插入密钥
            new_node->unused->value = value;//插入数据
            HeadInsertion(obj->head,new_node->unused);//更新最近使用的数据
            ++(obj->size);//缓存大小+1
        }
    }
    else
    {//密钥已存在
    // lRUCacheGet 函数已经更新双链表表头，故此处不用更新
        obj->head->next->value = value;//替换数据值
    }
}

void lRUCacheFree(LRUCache* obj) {
    free(obj->table);
    free(obj->head);
    free(obj->tail);
    free(obj);
}




//***********************************************************
//这里就不用读取文件了，毕竟老师的要求是缺页率
//**********************************************************


int main(void) {
    
    FILE *fp;
    int len;
    int addresses[15000];//开一个足bai够大的数组。
    int i = 0;
    char address_txt[100] = "addresses_my.txt";
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
    int map[128];
    struct node TLB[16];
    int TLBHitTimes = 0;
    int page_default = 0;
    //严格意义上说，这个就是页表
    int j ;
    for(int j = 0; j < 128 ; j++){
        map[j]  = -1;
    }
    //设定这个-1为我们未存取的情况
    int firstFram = 0;
    //当我们查找页表找不到的时候，我们就直接把这个变量分配给他，然后记录到map里面，++就好了
    //对于这个map我觉得，下标用page很方便。
    //也可以把这个数值理解为空闲帧与非空闲帧的分界线。
    int point = 0;
    int point_TLB = 0;
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
        int m = 0;
        
        int frame = -1;
        //实现map很麻烦，这里我们就反过来使用我们之前的map[]数组，key用来存放帧表，value放页表，搜查我们使用便利
        //在实际中，应该是使用map，rb_tree来实现这一对应的功能，但是老师说语言不是问题，我觉得完成任务，这二者是没有差别的
        
        //搜查TLB
        int w;
        for(w = 0; w < 16; w++){
            if(TLB[w].value == page ){
                TLBHitTimes++;
                printf("出现一次tlb命中   ");
                frame = TLB[w].key;
            }
        }


        //搜查页表
        if(frame == -1){
            int q;
            for(q = 0; q < 128;q++ ){
                if(map[q] == page){
                    //找到拉
                    frame = q;
                }
            }
            //进行页替换
            if(frame == -1){
                //没找到
                //实现先进先出的页替换，这里用队列比较好。
                printf("出现了一次缺页！   ");
                page_default++;
                if(point == 128){
                    point = 1;
                }            
                map[point] = page;
                frame = point;
                point++;
            }
            //更新TLB
            if(point_TLB == 16){
                point_TLB = 0;
            }
            TLB[point_TLB].value = page;
            TLB[point_TLB].key = frame;
            point_TLB++;
        }
        //现在我们得到了我们想要的数据，一个是偏移量，一个是对应的帧表数值，结果是要让我们返回二者和起来的数字。
        //frame + offset 和起来的数值，以及对应的value
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

        printf("TLBHitTimes %d   page_default %d   \n", TLBHitTimes,page_default);    
    }
    
    double TLB_p = (double)TLBHitTimes/10000;
    double page = (double)page_default/10000;
    printf("TLBHIT :  %lf   ,pageFalut: %lf \n" ,TLB_p ,page);
    return 0;
}










