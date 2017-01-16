using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace CreateIconWpfApplication
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.AllowDrop = true;
        }

        private void Window_Closed(object sender, EventArgs e)
        {

        }

        string GetFileName(string strFilename)
        {
            int nPos = strFilename.LastIndexOf('\\');
            int nLength = strFilename.Length;
            if (nPos < nLength)
                return strFilename.Substring(nPos + 1, (nLength - nPos) - 1);
            return strFilename;
        }

        string GetFileExtName(string strFilename)
        {
            int nPos = strFilename.LastIndexOf('.');
            int nLength = strFilename.Length;
            if (nPos < nLength)
                return strFilename.Substring(nPos + 1, (nLength - nPos) - 1);
            return string.Empty;
        }

        string GetFileNameOnly(string strFilename)
        {
            strFilename = GetFileName(strFilename);
            int nPos = strFilename.IndexOf(".");
            if (nPos != -1)
                strFilename = strFilename.Substring(0, nPos);
            return strFilename;
        }

        private static void CreateDirectory(string sPath)
        {
            if (!Directory.Exists(sPath))
            {
                Directory.CreateDirectory(sPath);
            }
        }

        private void Window_DragOver(object sender, DragEventArgs e)
        {  
            if (!e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effects = DragDropEffects.Move;
                e.Handled = true;
            }
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            string sPath = string.Format("{0}\\Icons", AppDomain.CurrentDomain.BaseDirectory);//Directory.GetCurrentDirectory());
            CreateDirectory(sPath);

            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                foreach (string file in files)
                {
                    string sNewFile = GetFileNameOnly(file);
                    string sNewPath = string.Format("{0}\\{1}.ico", sPath, sNewFile);
                    //BitmapSource pic = new BitmapImage(new Uri(file));
                    //BitmapImage pic = new BitmapImage(new Uri(file, UriKind.Absolute));
                    clsPngIcon.SaveToIcon(file, sNewPath);
                }
            }

            this.Close();
        }
    }
}
