#include <syscall.h>

int main()
{
    for(int i = 0; i < video_getheightscreen(); i++)
    {
        for(int j = 0; j < video_getwidthscreen(); j++)
        {
            video_putpixel(j, i, 0x00AABBCC);
        }
    }
    
    return 0x0;
}