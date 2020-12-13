using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinShellTerminal
{
	public class Command
	{
		public string Raw { get; private set; }
		public string Name { get; private set; }
		public string[] Args { get; private set; }

		public Command(string raw, string name, string[] args)
		{
			Raw = raw;
			Name = name;
			Args = args;
		}

		public string GetRaw(string commandFormat)
		{
			return string.Format(commandFormat, Raw);
		}

		public string GetName(string commandFormat)
		{
			return string.Format(commandFormat, Name);
		}

		public string GetArgs(string commandFormat)
		{
			string ret = "";
			for (int i = 1; i < Args.Length; i++)
				ret += string.Format(commandFormat, Args[i]);
			return ret;
		}

	}
}
