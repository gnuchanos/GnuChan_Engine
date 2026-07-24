using System;

namespace GnuChan
{
    /// <summary>
    /// Event arguments for when unhandled exceptions occur.
    /// </summary>
    public class UnhandledExceptionArgs
    {
        /// <summary>
        /// Exception object.
        /// </summary>
        public Exception Exception { get; private set; }

        internal UnhandledExceptionArgs(Exception exception)
        {
            Exception = exception;
        }
    }
}
