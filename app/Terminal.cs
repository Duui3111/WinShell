using System;
using System.Collections.Generic;
using System.Media;
using System.Windows.Controls;
using System.Windows.Input;

namespace WinShellTerminal
{
    public class Terminal : TextBox
    {
        protected enum CommandHistoryDirection { BACKWARD, FORWARD }
        public bool IsPromptInsertedAtLaunch { get; set; }
        public bool IsSystemBeepEnabled { get; set; }
        public string Prompt { get; set; }
        public List<Command> CommandLog { get; private set; }
        public int LastPomptIndex { get; private set; }
        public bool IsInputEnabled { get; private set; }

        private int indexInLog = 0;

        public Terminal()
        {
            IsUndoEnabled = false;
            AcceptsReturn = false;
            AcceptsTab = false;

            CommandLog = new List<Command>();
            IsPromptInsertedAtLaunch = true;
            IsSystemBeepEnabled = true;
            LastPomptIndex = -1;
            //   Prompt = "my promt > ";
            IsInputEnabled = false;

            Loaded += (s, e) =>
            {
                if (IsPromptInsertedAtLaunch)
                    InsertNewPrompt();
            };

            TextChanged += (s, e) =>
            {
                ScrollToEnd();
            };
        }

        public void InsertNewPrompt()
        {
            if (Text.Length > 0) Text += Text.EndsWith("\n") ? "" : "\n";
            Text += Prompt;
            CaretIndex = Text.Length;
            LastPomptIndex = Text.Length;
            IsInputEnabled = true;
        }

        public void InsertLineBeforePrompt(string text)
        {
            int oldPromptIndex = LastPomptIndex;
            String insertedText = text + (text.EndsWith("\n") ? "" : "\n");
            Text = Text.Insert(LastPomptIndex - Prompt.Length, insertedText);
            CaretIndex = Text.Length;
            LastPomptIndex = oldPromptIndex + insertedText.Length;
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.C)
            {
                if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
                {
                    RaiseAbortRequested();
                    e.Handled = true;
                    return;
                }
            }
            if (!IsInputEnabled)
            {
                if (IsSystemBeepEnabled) SystemSounds.Beep.Play();
                e.Handled = true;
                return;
            }

            if (CaretIndex < LastPomptIndex)
            {
                if (IsSystemBeepEnabled) SystemSounds.Beep.Play();
                CaretIndex = Text.Length;
                e.Handled = false;
                if (e.Key == Key.Back || e.Key == Key.Delete) e.Handled = true;
            }
            else if (CaretIndex == LastPomptIndex && e.Key == Key.Back || CaretIndex == LastPomptIndex && e.Key == Key.Left)
            {
                if (IsSystemBeepEnabled) SystemSounds.Beep.Play();
                e.Handled = true;
            }
            else if (CaretIndex >= LastPomptIndex && e.Key == Key.Up)
            {
                HandleCommandHistoryRequest(CommandHistoryDirection.BACKWARD);
                e.Handled = true;
            }
            else if (CaretIndex >= LastPomptIndex && e.Key == Key.Down)
            {
                HandleCommandHistoryRequest(CommandHistoryDirection.FORWARD);
                e.Handled = true;
            }

            if (!e.Handled)
            {
                switch (e.Key)
                {
                    case Key.Enter:
                        HandleEnterKey();
                        e.Handled = true;
                        break;
                }
            }

            base.OnPreviewKeyDown(e);
        }

        protected virtual void HandleCommandHistoryRequest(CommandHistoryDirection direction)
        {
            switch (direction)
            {
                case CommandHistoryDirection.BACKWARD:
                    if (indexInLog > 0)
                        indexInLog--;
                    if (CommandLog.Count > 0)
                    {
                        Text = GetTextWithPromptSuffix(CommandLog[indexInLog].Raw);
                        CaretIndex = Text.Length;
                    }
                    break;

                case CommandHistoryDirection.FORWARD:
                    if (indexInLog < CommandLog.Count - 1)
                        indexInLog++;
                    if (CommandLog.Count > 0)
                    {
                        Text = GetTextWithPromptSuffix(CommandLog[indexInLog].Raw);
                        CaretIndex = Text.Length;
                    }
                    break;
            }
        }

        protected virtual void HandleEnterKey()
        {
            string line = Text.Substring(LastPomptIndex);
            Text += "\n";
            IsInputEnabled = false;
            LastPomptIndex = int.MaxValue;

            Command cmd = TerminalUtils.ParseCommandLine(line);
            CommandLog.Add(cmd);
            indexInLog = CommandLog.Count;
            RaiseCommandEntered(cmd);
        }

        protected string GetTextWithPromptSuffix(string suffix)
        {
            string ret = Text.Substring(0, LastPomptIndex);
            return ret + suffix;
        }

        public event EventHandler<EventArgs> AbortRequested;
        public event EventHandler<CommandEventArgs> CommandEntered;

        public class CommandEventArgs : EventArgs
        {
            public Command Command { get; private set; }
            public CommandEventArgs(Command command) => Command = command;
        }

        private void RaiseAbortRequested() => AbortRequested?.Invoke(this, new EventArgs());

        private void RaiseCommandEntered(Command command) => CommandEntered?.Invoke(this, new CommandEventArgs(command));
    }
}
