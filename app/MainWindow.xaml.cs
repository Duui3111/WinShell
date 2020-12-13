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
using System.Windows.Threading;
using System.IO;
using Microsoft.Win32;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using Microsoft;

namespace WinShellTerminal
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Term.Prompt = $"\n {Directory.GetCurrentDirectory()}  4 $ ";

            Loaded += (s, e) => {
                Term.CommandEntered += (ss, ee) => {
                    string msgmsg = ee.Command.GetRaw("{0}");
                    Term.InsertNewPrompt();
                    Process process = new Process();
                    process.StartInfo.FileName = "cmd.exe";
                    process.StartInfo.Arguments = "/c " + msgmsg;
                    process.StartInfo.UseShellExecute = false;
                    process.StartInfo.RedirectStandardOutput = true;
                    process.StartInfo.RedirectStandardError = true;
                    process.StartInfo.CreateNoWindow = true;
                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    Term.InsertLineBeforePrompt(output);
                    string err = process.StandardError.ReadToEnd();
                    Term.InsertLineBeforePrompt(err);
                    process.WaitForExit();

                };

                Term.AbortRequested += (ss, ee) => {
                    MessageBox.Show("Abort !");
                };

                Term.InsertNewPrompt();
            };
        }
    }
}
