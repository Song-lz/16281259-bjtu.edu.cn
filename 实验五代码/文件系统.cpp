#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <malloc.h>
//�����Ĵ�С���ֽڣ�
#define SECTOR_SIZE 512
//������
#define SECTORS_NUM 2880
//��������ֽ���
#define DATA_MAX_SIZE 2847*512
//Ŀ¼����
#define DIR_ITEM_NUM 224
//�������Ĵ�С���ֽڣ�
char buffer[SECTOR_SIZE];
//����ӳ���ļ�
FILE *fat32;

int format(FILE* f);                //fat32ӳ���ļ���ʽ��

int read_block(int i, char *p, FILE* f);

int write_block(int i, char *p, FILE* f);

int disk();             //���������ļ�

int create();

int read_file();

int rename_file();

int destroy();

int directory();


int main()
{
    char cmdbuf[50]="";
    printf("File System��type 'help' for more information.");
    while (1)
    {
        printf("\nC:User\\fyb>");
        scanf("%s", cmdbuf);
        getchar();
        if (!strcmp(cmdbuf, "help"))
        {
            printf("the max length of file name is 8-bits                     \n");
            printf("disk                                             disk operation GUI\n");
            printf("create           file.ext                        create file\n");
            printf("read_file        file.ext                        read file\n");
            printf("rename_file      ori_file  new_file              rename file\n");
            printf("destroy          file.ext                        delete file\n");
            printf("create_dir       sub_dir                         create subdir\n");
            printf("delete_dir       sub_dir                         delete subdir\n");
            printf("directory                                        show directory and file recursively\n");
            printf("exit                                             logout\n");
            printf("help                                             help command\n");
        }
        //exit���˳�ģ���ļ�����ϵͳ
        else if (!strcmp(cmdbuf, "exit"))
        {
            break;
        }
        //create:����һ���ļ�
        else if (!strcmp(cmdbuf, "create"))
        {
            create();
        }
        //disk:������ز���
        else if (!strcmp(cmdbuf, "disk"))
        {
            disk();
        }
        else if (!strcmp(cmdbuf, "read_file"))
        {
            read_file();
        }
        else if (!strcmp(cmdbuf, "rename_file"))
        {
            rename_file();
        }
        else if (!strcmp(cmdbuf, "destroy"))
        {
            destroy();
        }
        else if (!strcmp(cmdbuf, "create_dir"))
        {
            create_dir();
        }
        else if (!strcmp(cmdbuf, "delete_dir"))
        {
            delete_dir();
        }
        else if (!strcmp(cmdbuf, "directory"))
        {
            directory();
        }

        else{printf("\nIllegal command��Type 'help' for more info.\n");}

    }

    return 0;
}


int format(FILE* f)
{
    //���������ṹ
    //������С���ֽڣ�
    fseek(f, 11, SEEK_SET);
    fprintf(f, "\x02");
    fseek(f, 12, SEEK_SET);
    fprintf(f, "\x00");
    //�ش�С
    fseek(f, 13, SEEK_SET);
    fprintf(f, "\x01");
    //����������
    fseek(f, 14, SEEK_SET);
    fprintf(f, "\x00");
    fseek(f, 15, SEEK_SET);
    fprintf(f, "\x01");
    //�ļ������FAT��Ϊ
    fseek(f, 16, SEEK_SET);
    fprintf(f, "\x01");
    //��Ŀ¼��С
    fseek(f, 17, SEEK_SET);
    fprintf(f, "\x00");
    fseek(f, 18, SEEK_SET);
    fprintf(f, "\xe0");
    //���̾���������
    fseek(f, 19, SEEK_SET);
    fprintf(f, "\x0b");
    fseek(f, 20, SEEK_SET);
    fprintf(f, "\x40");
    //�ļ�������СΪ����
    fseek(f, 22, SEEK_SET);
    fprintf(f, "\x00");
    fseek(f, 23, SEEK_SET);
    fprintf(f, "\x09");
    //ÿ�ŵ��������趨Ϊx12
    fseek(f, 24, SEEK_SET);
    fprintf(f, "\x00");
    fseek(f, 25, SEEK_SET);
    fprintf(f, "\x12");
    //��ͷ��
    fseek(f, 26, SEEK_SET);
    fprintf(f, "\x00");
    fseek(f, 27, SEEK_SET);
    fprintf(f, "\x02");
    //����ǩ��ȡֵΪx29
    fseek(f, 38, SEEK_SET);
    fprintf(f, "\x29");
    //���ʶ��
    fseek(f, 39, SEEK_SET);
    fprintf(f, "X");
    //�����
    fseek(f, 43, SEEK_SET);
    fprintf(f, "fish");
    //�ļ�ϵͳ����
    fseek(f, 54, SEEK_SET);
    fprintf(f, "fat32");

    printf("ISO file format successfully~\n");
    return 0;
}

int read_block(int i, char *p, FILE* f)
{
    fseek(f, i*SECTOR_SIZE, SEEK_SET);
    int j=0;
    fscanf(f, "%c", &p[j]);
    while(p[j]!='\0' && j< SECTOR_SIZE)
    {
        j++;
        fscanf(f, "%c", &p[j]);
    }
    rewind(f);
    printf("input buffer:%s", p);
    return 0;
}
int write_block(int i, char *p, FILE* f)
{
    fseek(f, i*SECTOR_SIZE, SEEK_SET);
    fwrite(p, sizeof(char), SECTOR_SIZE, f);
    rewind(f);
    memset(p, '\0', SECTOR_SIZE);
    return 0;
}

int disk()
{
    //���������ļ�
    char buf[SECTORS_NUM*SECTOR_SIZE]=" ";
    char ch;
    int i, num=0;
    printf("Create ISO file .....\n");
    fat32 = fopen("fish.img", "wb");
    if(fat32 == NULL)
    {
        printf("Create ISO failed~\n");
        return 3;
    }
    memset(buf, '\x0', SECTORS_NUM*SECTOR_SIZE);
    fwrite(buf, sizeof(char), SECTOR_SIZE*SECTORS_NUM, fat32);
    printf("Create ISO file successfully��\n");

    //��ʽ�������ļ�
    printf("ISO file format.....\n");
    rewind(fat32);
    format(fat32);
    rewind(fat32);

    //ͨ���������������̿���������
    printf("***************emulation*********************\n");
    printf("��input data from buffer into physical block��\n");
    printf("input block number��range��33��2879��\n");
    scanf("%d", &i);
    getchar();
    printf("input buffer data\n");
    //�򻺳�����������
    scanf("%c",&ch);
    while(ch!= '\n' && num< SECTOR_SIZE)
    {
        buffer[num]= ch;
        num++;
        scanf("%c",&ch);
    }
    //���������е�����д�������̿�
    write_block(i, buffer, fat32);
    fclose(fat32);
    //�鿴�������Ƿ��ÿ�
    //printf("%s\n", buffer);

    printf("***************emulation*********************\n");
    printf("��input data from buffer into physical block��\n");
    printf("input block number��range��33��2879��\n");
    scanf("%d", &i);
    getchar();
    fat32 = fopen("fish.img", "rb");
    if(fat32 == NULL)
    {
        printf("open ISO failed~\n");
        return 4;
    }
    //�������̿��е����ݶ��뻺����
    read_block(i, buffer, fat32);
    //�ر�fat32�ļ�
    fclose(fat32);
}

int create()
{
    //��������
    char fullname[30]="";
    char *split = ".";
    char *s= NULL;
    char *name[2]={NULL,NULL};
    char text[DATA_MAX_SIZE]="";
    char *ll=NULL;
    char tmp2= 's';
    char date[50]="";
    char temp[20]="";
    char y[10]="";
    char tm[50]="";
    char t[50]="";
    char nn[9]="";
    int length, SectionNum,i,j,tmp,k,menu,rlength,kk,nl,f,a,b,pro;
    short int fat[2847], n, half;

    scanf("%s", fullname);
    getchar();
    s = strtok (fullname,split);
    name[0]= s;
    s= strtok (NULL,split);
    name[1]= s;
    //�ļ�����Сдת��
    nl=strlen(name[0]);
    for(kk=0; kk<nl; kk++)
    {
        if(name[0][kk]>='a'&&name[0][kk]<='z')
            name[0][kk]-=32;
    }
    //�Կɶ�д��ʽ���ļ�
    fat32 = fopen("fish.img", "r+");

    //��Ŀ¼���в����������Ƿ��ѱ������ļ�ʹ��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            nn[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(nn, name[0])==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            if(pro!=0x10)
            {
                printf("file name has been used��create failed��\n");
                return 3;
            }
        }
        for(j=0; j<8; j++)
        {
            nn[j]='\0';
        }
    }

    printf("input file contents:\n");
    scanf("%s", text);
    length = strlen(text);
    printf("file length��%x\n",length);
    SectionNum = length/SECTOR_SIZE+1;

    tmp2= 's';
    //Ѱ���п���λ�õ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM && ('\0' != tmp2); i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        tmp2 = getc(fat32);
    }
    menu= tmp;

    //Ѱ��FAT1�ļ�������п��е�λ�ã���д������
    i=2;
    for(j=0; j<SectionNum; j++)
    {
        f=2;
        for (; i<DIR_ITEM_NUM && f!=0; i++)
        {
            if(i%2==0)//�ȶ�ȡ���ֽ�
            {
                tmp = SECTOR_SIZE+i*1.5;
                fseek(fat32, tmp, SEEK_SET);
                a = getc(fat32);//���ֽ�
                b = getc(fat32);//���ֽ�
                b=b&0xF0;
                a=a<<8;
                f=(a|b)>>4;
            }
            if(i%2==1)//�ȶ�ȡ���ֽ�
            {
                tmp = SECTOR_SIZE+i*1.5;
                fseek(fat32, tmp, SEEK_SET);
                a = getc(fat32);//���ֽ�
                a=a&0x0F;
                a= a<<8;
                b = getc(fat32);//���ֽ�
                f=a|b;
            }


        }
        fat[j]= i-1;
        //���һ��
        if(j!=0)
        {
            //д��һ���ص��߼��غ�
            //ż�����ֽڣ���д���ֽ���д���ֽ�
            if(fat[j-1]%2==0)
            {
                tmp = SECTOR_SIZE+(fat[j-1])*1.5;
                fseek(fat32, tmp, SEEK_SET);
                n=fat[j];
                n = n >> 4;
                n&= 0xFF;//ǰ8λ
                putc(n, fat32);
                n= fat[j];
                n= n << 4;
                n&=0xF0;//��4λ
                putc(n,fat32);
            }
            //�������ֽڣ���д���ֽ���д���ֽ�
            if(fat[j-1]%2==1)
            {
                tmp = SECTOR_SIZE+(fat[j-1])*1.5;
                fseek(fat32, tmp, SEEK_SET);
                half= getc(fat32);
                n=fat[j];
                n = n >> 8;
                fseek(fat32, tmp, SEEK_SET);
                putc(n|half, fat32);//ǰ4λ����һ��1.5�ֽڵĺ�4λ���
                n= fat[j];
                n&=0xFF;//��8λ
                putc(n,fat32);
            }

        }
        //���һ����дFFF
        if(j==(SectionNum-1))
        {
            if(fat[j]%2==0)
            {
                tmp = SECTOR_SIZE+fat[j]*1.5;
                fseek(fat32, tmp, SEEK_SET);
                fwrite("\xFF", 1, 1, fat32);
                fwrite("\xF0", 1, 1, fat32);
            }
            if(fat[j]%2==1)
            {
                tmp = SECTOR_SIZE+fat[j]*1.5;
                fseek(fat32, tmp, SEEK_SET);
                half= getc(fat32);
                fseek(fat32, tmp, SEEK_SET);
                putc(0x0F|half, fat32);
                fwrite("\xFF", 1, 1, fat32);
            }

        }
    }
    k=0;
    ll= text;
    //������д���Ӧ�������̿鼴��������Ӧλ��
    for(j=0; j<SectionNum; j++)
    {
        fseek(fat32,SECTOR_SIZE*(33 + fat[j] -2), SEEK_SET);
        fwrite(ll, sizeof(char), SECTOR_SIZE, fat32);
        ll+= SECTOR_SIZE;
    }

    //����Ŀ¼��
    //�ļ���
    fseek(fat32, menu, SEEK_SET);
    fprintf(fat32, name[0]);
    //�ļ���չ��
    fseek(fat32, menu+8, SEEK_SET);
    fprintf(fat32, name[1]);
     //��һ�߼��غ�2�ֽ�
    fseek(fat32, menu+26, SEEK_SET);
    n= fat[0];
    n =n >> 8;
    n&= 0x0F;//��λ�ֽ�
    putc(n, fat32);
    n= fat[0];
    n&=0xFF;//��λ�ֽ�
    putc(n, fat32);

    //�ļ���С4�ֽ�
    //1
    fseek(fat32, menu+31, SEEK_SET);
    rlength = length%256;
    putc(rlength, fat32);
    //2
    fseek(fat32, menu+30, SEEK_SET);
    length /= 256;
    rlength = length%256;
    putc(rlength, fat32);
    //3
    fseek(fat32, menu+29, SEEK_SET);
    length /= 256;
    rlength = length%256;
    putc(rlength, fat32);
    //4
    fseek(fat32, menu+28, SEEK_SET);
    length /= 256;
    rlength = length%256;
    putc(rlength, fat32);

    //�ر�ӳ���ļ�
    fclose(fat32);
    printf("File create successfully��\n");
    return 0;
}



int read_file()
{
    char fullname[30]="";
    char *split = ".";
    char *s= NULL;
    char *name=NULL;
    char tmp2;
    char n[9]="";
    char text[DATA_MAX_SIZE]="";
    int i,j,numl,numh,nl,kk,loc,flag=0,tmp,pro;
    int a,b;
    //�Կɶ���ʽ���ļ�
    fat32 = fopen("fish.img", "r");
    scanf("%s", fullname);
    getchar();
    s = strtok (fullname,split);
    name= s;
    //�ļ�����Сдת��
    nl=strlen(name);
    for(kk=0; kk<nl; kk++)
    {
        if(name[kk]>='a' && name[kk]<='z')
            name[kk]-=32;
    }
    //��Ŀ¼���в��Ҹ����ֵ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
         tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            n[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(n, name)==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            //������Ŀ¼
            if(pro!=0x10)
            {
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp+26, SEEK_SET);
                numh= getc(fat32);//��һ�߼��غŸ��ֽ�
                numl= getc(fat32);//��һ�߼��غŵ��ֽ�
                loc= 256*numh+ numl;//��һ�߼��غ�
                while(loc!=0xFFF)
                {
                    fseek(fat32,SECTOR_SIZE*(33 + loc -2), SEEK_SET);
                    fread(text, SECTOR_SIZE, 1,fat32);
                    printf("%s",text);
                    //Ѱ����һ�߼���
                    tmp = SECTOR_SIZE+loc*1.5;
                    fseek(fat32, tmp, SEEK_SET);
                    if(loc%2==0)//�ȶ����ֽ�
                    {
                        a=getc(fat32);
                        b=getc(fat32);
                        b=b&0xF0;
                        loc=(a<<8)|b;
                        loc= loc>>4;
                    }
                    else//�ȶ����ֽ�
                    {
                        a=getc(fat32);
                        a= a&0x0F;
                        a=a<<8;
                        b=getc(fat32);
                        loc=a|b;
                    }
                }
                flag=1;
                break;
            }
        }
        for(j=0; j<8; j++)
        {
            n[j]='\0';
        }
    }

    if(flag==0)
        printf("The file doesn't exist\n");

    //�ر��ļ�
    fclose(fat32);
    return 0;
}
int rename_file()
{
    char oldname[30]="";
    char newname[30]="";
    char *split = ".";
    char *s1= NULL;
    char *s2= NULL;
    char *oldn= NULL;
    char *newn=NULL;
    char tmp2;
    char n[9]="";
    int i,j,nl,kk,flag=0,tmp,pro;
    //�Կɶ�д��ʽ���ļ�
    fat32 = fopen("fish.img", "r+");
    scanf("%s", oldname);
    getchar();
    s1 = strtok (oldname,split);
    oldn= s1;
    scanf("%s", newname);
    getchar();
    s2 = strtok (newname,split);
    newn= s2;
    //�ļ�����Сдת��
    nl=strlen(oldn);
    for(kk=0; kk<nl; kk++)
    {
        if(oldn[kk]>='a'&&oldn[kk]<='z')
            oldn[kk]-=32;
    }
    nl=strlen(newn);
    for(kk=0; kk<nl; kk++)
    {
        if(newn[kk]>='a'&&newn[kk]<='z')
            newn[kk]-=32;
    }
    if(strcmp(newn, oldn)==0)
    {
        printf("The file name failed to rename~\n");
        return 0;
    }
    //��Ŀ¼���в����������Ƿ��ѱ������ļ�ʹ��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
         tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            n[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(n, newn)==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            if(pro!=0x10)
            {
                printf("Rename failed��\n");
                return 3;
            }
        }
        for(j=0; j<8; j++)
        {
            n[j]='\0';
        }
    }

    //��Ŀ¼���в��Ҹ����ֵ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        //fscanf(fat32,"%c",&tmp2);
        tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            n[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(n, oldn)==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            if(pro!=0x10)
            {
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp, SEEK_SET);
                fwrite(newn,8,1,fat32);
                flag=1;
                break;
            }
        }
        for(j=0; j<8; j++)
        {
            n[j]='\0';
        }
    }

    if(flag==0)
        printf("The file doesn't exist\n");
    else
        printf("Rename successfully��\n");

    //�ر��ļ�
    fclose(fat32);
    return 0;
}
int destroy()
{
    char fullname[30]="";
    char *split = ".";
    char *s= NULL;
    char *name=NULL;
    char tmp2;
    char n[9]="";
    int i,j,numl,numh,nl,kk,loc,dloc,flag=0,tmp,pro;
    int a,b,cc;
    char buf[SECTOR_SIZE];
    memset(buf, '\x0', SECTOR_SIZE);
    //�Կɶ�д��ʽ���ļ�
    fat32 = fopen("fish.img", "r+");
    scanf("%s", fullname);
    getchar();
    s = strtok (fullname,split);
    name= s;
    //�ļ�����Сдת��
    nl=strlen(name);
    for(kk=0; kk<nl; kk++)
    {
        if(name[kk]>='a' && name[kk]<='z')
            name[kk]-=32;
    }
    //��Ŀ¼���в��Ҹ����ֵ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
         tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            n[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(n, name)==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            //������Ŀ¼
            if(pro!=0x10)
            {
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp+26, SEEK_SET);
                numh= getc(fat32);//��һ�߼��غŸ��ֽ�
                numl= getc(fat32);//��һ�߼��غŵ��ֽ�
                loc= 256*numh+ numl;//��һ�߼��غ�
                while(loc!=0xFFF)
                {
                    fseek(fat32,SECTOR_SIZE*(33 + loc -2), SEEK_SET);
                    //���������
                    fwrite(buf, sizeof(char), SECTOR_SIZE, fat32);
                    //Ѱ����һ�߼���
                    dloc=loc;
                    tmp = SECTOR_SIZE+loc*1.5;
                    fseek(fat32, tmp, SEEK_SET);
                    //���fat�ļ���
                    if(loc%2==0)//�ȶ����ֽ�
                    {
                        a=getc(fat32);
                        b=getc(fat32);
                        fseek(fat32, tmp, SEEK_SET);
                        putc(0x00,fat32);//������ֽ�
                        cc=b;
                        cc=cc&0x0F;
                        putc(cc,fat32);//��հ��ֽ�
                        b=b&0xF0;
                        loc=(a<<8)|b;
                        loc= loc>>4;
                    }
                    else//�ȶ����ֽ�
                    {
                        a=getc(fat32);
                        b=getc(fat32);
                        cc=a;
                        cc=cc&0xF0;
                        fseek(fat32, tmp, SEEK_SET);
                        putc(cc,fat32);//��հ��ֽ�
                        putc(0x00,fat32);//������ֽ�
                        a= a&0x0F;
                        a=a<<8;
                        loc=a|b;
                    }

                }
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp, SEEK_SET);
                //���Ŀ¼��
                fwrite(buf, sizeof(char), 32, fat32);
                flag=1;
                break;
            }
        }
        for(j=0; j<8; j++)
        {
            n[j]='\0';
        }
    }

    if(flag==0)
        printf("The file doesn't exist\n");
    else
        printf("The file delete successfully~\n");

    //�ر��ļ�
    fclose(fat32);
    return 0;
}
int create_dir()
{
    char dirname[9]="";
    char n[9]="";
    int tmp,menu,i,pro,j,nl,kk;
    char tmp2;
    fat32 = fopen("fish.img", "r+");
    scanf("%s", dirname);
    getchar();
    nl=strlen(dirname);
    for(kk=0; kk<nl; kk++)
    {
        if(dirname[kk]>='a'&& dirname[kk]<='z')
             dirname[kk]-=32;
    }
    //�鿴������Ŀ¼
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        tmp2 = getc(fat32);
        if('\0' != tmp2)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            if(pro==0x10)
            {
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp, SEEK_SET);
                for(j=0; j<8; j++)
                {
                    tmp2 = getc(fat32);
                    n[j]= tmp2;
                }
                if(strcmp(n, dirname)==0)
                {
                    printf("The directory exists, create failed......\n");
                    return 1;
                }
                for(j=0; j<8; j++)
                {
                    n[j]='\0';
                }
            }
        }

    }
    tmp2='s';
    //Ѱ���п���λ�õ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM && ('\0' != tmp2); i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        tmp2 = getc(fat32);
    }
    menu= tmp;
    //����Ŀ¼��
    //Ŀ¼��
    fseek(fat32, menu, SEEK_SET);
    fprintf(fat32, dirname);
    //�ļ�����
    fseek(fat32, menu+11, SEEK_SET);
    fprintf(fat32, "\x10");
    printf("Sub directory create successfully��\n");

    //�ر�ӳ���ļ�
    fclose(fat32);
    return 0;
}
int delete_dir()
{
    char dirname[9]="";
    char *split = ".";
    char *s= NULL;
    char *name=NULL;
    char tmp2;
    char n[9]="";
    int i,j,nl,kk,flag=0,tmp,pro;
    char buf[SECTOR_SIZE];
    memset(buf, '\x0', SECTOR_SIZE);
    //�Կɶ�д��ʽ���ļ�
    fat32 = fopen("fish.img", "r+");
    scanf("%s", dirname);
    getchar();
    //Ŀ¼����Сдת��
    nl=strlen(dirname);
    for(kk=0; kk<nl; kk++)
    {
        if(dirname[kk]>='a' && dirname[kk]<='z')
            dirname[kk]-=32;
    }
    //��Ŀ¼���в��Ҹ����ֵ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        //fscanf(fat32,"%c",&tmp2);
        tmp2 = getc(fat32);
        //�����ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            n[j]= tmp2;
            tmp2 = getc(fat32);
        }
        if(strcmp(n, dirname)==0)
        {
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            //��Ŀ¼
            if(pro==0x10)
            {
                tmp = 19*SECTOR_SIZE+i*32;
                fseek(fat32, tmp, SEEK_SET);
                //���Ŀ¼��
                fwrite(buf, sizeof(char), 32, fat32);
                flag=1;
                break;
            }
        }
        for(j=0; j<8; j++)
        {
            n[j]='\0';
        }
    }

    if(flag==0)
        printf("The directory doesn't exist\n");
    else
        printf("The directory deleted successfully~\n");

    //�ر��ļ�
    fclose(fat32);
    return 0;
}
int directory()
{
    char exp[9]="";
    char tmp2;
    char name[9]="";
    int i,j,nl,tmp,pro,flag;
    //�Կɶ�д��ʽ���ļ�
    fat32 = fopen("fish.img", "r+");

    //��Ŀ¼���в��Ҹ����ֵ�Ŀ¼��
    for (i=0; i<DIR_ITEM_NUM; i++)
    {
        flag=0;
        tmp = 19*SECTOR_SIZE+i*32;
        fseek(fat32, tmp, SEEK_SET);
        tmp2 = getc(fat32);
        //�ļ���
        for(j=0; j<8 && ('\0' != tmp2); j++)
        {
            name[j]= tmp2;
            tmp2 = getc(fat32);
            flag=1;
        }
        if(flag==1)
        {
            printf("%s",name);
            nl=strlen(name);
            nl=8-nl;
            for(j=0;j<nl;j++)
            {
                printf(" ");
            }
            for(j=0; j<8; j++)
            {
                name[j]='\0';
            }
            tmp = 19*SECTOR_SIZE+i*32+11;
            fseek(fat32, tmp, SEEK_SET);
            pro=getc(fat32);
            //����Ŀ¼
            if(pro==0x10) printf("dir\n");
            else
            {
                //�ļ���׺��
                tmp = 19*SECTOR_SIZE+i*32+8;
                fseek(fat32, tmp, SEEK_SET);
                tmp2 = getc(fat32);
                for(j=0; j<3 && ('\0' != tmp2); j++)
                {
                    exp[j]= tmp2;
                    tmp2 = getc(fat32);
                }
                printf("%s\n",exp);
                for(j=0; j<3; j++)
                {
                    exp[j]='\0';
                }
            }

        }

    }

    //�ر��ļ�
    fclose(fat32);
    return 0;
}



