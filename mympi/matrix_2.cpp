#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;

typedef chrono::high_resolution_clock Clock;

const int maxn=2100;

int N,M,K;

int main()
{
	M=512;
	N=512;
	K=512;
	int size=512;
	int *a,*b,*c,*buffer,*ans,*c2;
	int rank,numprocs,part;
	a = (int*)malloc(sizeof(int)*size*size);
    b = (int*)malloc(sizeof(int)*size*size);
    c = (int*)malloc(sizeof(int)*size*size); 
    c2 = (int*)malloc(sizeof(int)*size*size); 
    
	MPI_Init(NULL,NULL);//MPI Initialize
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);//��õ�ǰ���̺�
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);//��ý��̸���
    part=size/numprocs;
    buffer=(int*)malloc(sizeof(int)*size*part);
    ans=(int*)malloc(sizeof(int)*size*part);
	if(rank==0)
	{
		//generate the data
		auto stti = Clock::now();
		srand(time(0));
		for(int i=1;i<=M;i++)
		{
			for(int j=1;j<=N;j++)
			{
				a[(i-1)*size+(j-1)]=rand()%10;
			}
		}
		for(int i=1;i<=N;i++)
		{
			for(int j=1;j<=K;j++)
			{
				b[(i-1)*size+(j-1)]=rand()%10;
			}
		}
		auto endti = Clock::now();
		auto dur = chrono::duration_cast<chrono::microseconds>(endti - stti);
		cout << "Generate time:" << dur.count() << "microseconds" << endl;

        for(int i=1;i<=M;i++)
	    {
		    for(int j=1;j<=K;j++)
		    {
			    for(int k=1;k<=N;k++)
			    {
				    c2[(i-1)*size+j-1]+=a[(i-1)*size+k-1]*b[(k-1)*size+j-1];
			    } 
		    }
	    }
        int sum1=0.0;
        for(int i=1;i<=M;i++)
        {
            for(int j=1;j<=K;j++)
            {
                sum1+=c2[(i-1)*size+j-1];
            }
        }
		
		auto stti2 = Clock::now();
        MPI_Scatter(a,size*part,MPI_INT,buffer,size*part,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Bcast(b,size*size,MPI_INT,0,MPI_COMM_WORLD);
        for(int i=0;i<part;i++)
        {
            for(int j=0;j<size;j++)
            {
                int sum=0;
                for(int k=0;k<size;k++)
                {
                    sum+=buffer[i*size+k]*b[k*size+j];
                }
                ans[i*size+j]=sum;
            }
        }
        
        free(buffer);
        
        MPI_Gather(ans,part*size,MPI_INT,c,part*size,MPI_INT,0,MPI_COMM_WORLD);

        int rest=size%numprocs;
        if(rest)
        {
            for(int i=size-rest-1;i<size;i++)
            {
                for(int j=0;j<size;j++)
                {
                    double sum=0.0;
                    for(int k=0;k<size;k++)
                    {
                        sum+=a[i*size+k]*b[k*size+j];
                    }
                    c[i*size+j]=sum;
                }
            }
        }
        auto endti2 = Clock::now();
        auto dur2 = chrono::duration_cast<chrono::microseconds>(endti2 - stti2);
        cout << "Compute time:" << dur2.count() << "microseconds" << endl;
        int sum2=0;
        for(int i=1;i<=M;i++)
        {
            for(int j=1;j<=K;j++)
            {
                sum2+=c[(i-1)*size+j-1];
            }
        }
        cout<<sum1<<" "<<sum2<<endl;
        free(a);
        free(b);
        free(c);
        free(ans);
	}
	
	else
	{
        MPI_Scatter(a,part*size,MPI_INT,buffer,part*size,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(b,size*size,MPI_INT,0,MPI_COMM_WORLD);
		for (int i=0;i<part;i++)
        {
            for (int j=0;j<size;j++)
            {
                double temp=0;
                for(int k=0;k<size;k++)
                    temp += buffer[i*size+k]*b[k*size+j];
                ans[i*size+j]=temp;
            }
        }
        MPI_Gather(ans,part*size,MPI_INT,c,part*size,MPI_INT,0,MPI_COMM_WORLD);
        free(buffer);
        free(b);
        free(ans);
	}
	
	MPI_Finalize();
	
	
	
}
