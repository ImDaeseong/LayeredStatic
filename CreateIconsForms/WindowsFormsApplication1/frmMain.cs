using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace WindowsFormsApplication1
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            MoveLocationDialog();
        }

        private void MoveLocationDialog()
        {
            int nScreenWidth = Screen.PrimaryScreen.Bounds.Width;
            int nScreenHeight = Screen.PrimaryScreen.Bounds.Height;
            int nWidht = (nScreenWidth - this.Width);
            int nHeight = (nScreenHeight - this.Height);
            this.Location = new Point((nWidht / 2), (nHeight / 2));
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
        
        private void frmMain_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Move;
            }
        }
        
        private void frmMain_DragDrop(object sender, DragEventArgs e)
        {
            string sPath = string.Format("{0}\\Icons", Application.StartupPath);
            CreateDirectory(sPath);

            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                foreach (string file in files)
                {                    
                    string sNewFile = GetFileNameOnly(file);
                    string sNewPath = string.Format("{0}\\{1}.ico", sPath, sNewFile);

                    Image pic = Bitmap.FromFile(file);
                    clsPngIcon.SaveToIcon(pic, new Size(128, 128) ,sNewPath);
                }
            }

            this.Close();
        }

       
    }
}
