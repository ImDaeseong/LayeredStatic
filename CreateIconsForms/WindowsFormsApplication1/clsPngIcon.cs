/*
CreateIconLib
Copyright (C) 2016
https://github.com/scimence/easyIcon/blob/master/easyIcon/easyIcon/IconTool.cs
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace WindowsFormsApplication1
{
    class IconInfo
    {
        public IconInfo()
        {
        }
        public byte Width = 16;         
        public byte Height = 16;       
        public byte ColorNum = 0;       
        public byte Reserved = 0;     
        public ushort Planes = 1;       
        public ushort PixelBit = 32;   

        public uint ImageSize = 0;      
        public uint ImageOffset = 0; 

        public byte[] ImageData;     
    }

    class clsPngIcon
    {
        public static IconInfo creatIconInfo(Image pic, Rectangle rect)
        {
            int w = pic.Width > 255 ? 255 : pic.Width;
            int h = pic.Height > 255 ? 255 : pic.Height;
            if (rect == Rectangle.Empty || rect.Width > 255 || rect.Height > 255) rect = new Rectangle(0, 0, w, h);
            
            Bitmap IconBitmap = new Bitmap(rect.Width, rect.Height);
            Graphics g = Graphics.FromImage(IconBitmap);
            g.DrawImage(pic, rect, new Rectangle(0, 0, pic.Width, pic.Height), GraphicsUnit.Pixel);
            g.Dispose();

            System.IO.MemoryStream memoryStream = new System.IO.MemoryStream();
            IconBitmap.Save(memoryStream, System.Drawing.Imaging.ImageFormat.Bmp);

            IconInfo iconInfo1 = new IconInfo();
            iconInfo1.Width = (byte)rect.Width;
            iconInfo1.Height = (byte)rect.Height;

            memoryStream.Position = 14;
            iconInfo1.ImageData = new byte[memoryStream.Length - memoryStream.Position];
            memoryStream.Read(iconInfo1.ImageData, 0, iconInfo1.ImageData.Length);

            byte[] Height = BitConverter.GetBytes((uint)iconInfo1.Height * 2);
            iconInfo1.ImageData[8] = Height[0];
            iconInfo1.ImageData[9] = Height[1];
            iconInfo1.ImageData[10] = Height[2];
            iconInfo1.ImageData[11] = Height[3];

            iconInfo1.ImageSize = (uint)iconInfo1.ImageData.Length;
            iconInfo1.ImageOffset = 6 + (uint)(1 * 16);

            return iconInfo1;
        }
                
        public static void SaveToIcon(Image pic, string FileName)
        {
            SaveToIcon(pic, Rectangle.Empty, FileName);
        }

        public static void SaveToIcon(Image pic, Size size, string PathName)
        {
            SaveToIcon(pic, new Rectangle(0, 0, size.Width, size.Height), PathName);
        }

        public static void SaveToIcon(Image pic, Rectangle rect, string PathName)
        {
            if (rect.Width > 255 || rect.Height > 255) return;

            IconInfo iconInfo = creatIconInfo(pic, rect);

            System.IO.FileStream stream = new System.IO.FileStream(PathName, System.IO.FileMode.Create);

            ushort Reserved = 0;
            ushort Type = 1;
            ushort Count = 1;

            byte[] Temp = BitConverter.GetBytes(Reserved);
            stream.Write(Temp, 0, Temp.Length);
            Temp = BitConverter.GetBytes(Type);
            stream.Write(Temp, 0, Temp.Length);
            Temp = BitConverter.GetBytes((ushort)Count);
            stream.Write(Temp, 0, Temp.Length);

            stream.WriteByte(iconInfo.Width);
            stream.WriteByte(iconInfo.Height);
            stream.WriteByte(iconInfo.ColorNum);
            stream.WriteByte(iconInfo.Reserved);
            Temp = BitConverter.GetBytes(iconInfo.Planes);
            stream.Write(Temp, 0, Temp.Length);
            Temp = BitConverter.GetBytes(iconInfo.PixelBit);
            stream.Write(Temp, 0, Temp.Length);
            Temp = BitConverter.GetBytes(iconInfo.ImageSize);
            stream.Write(Temp, 0, Temp.Length);
            Temp = BitConverter.GetBytes(iconInfo.ImageOffset);
            stream.Write(Temp, 0, Temp.Length);

            stream.Write(iconInfo.ImageData, 0, iconInfo.ImageData.Length);

            stream.Close();
        }
    }
}
