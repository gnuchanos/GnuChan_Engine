using System;

namespace GnuChan
{
    [AttributeUsage(AttributeTargets.Method)]
    internal class GodotMethodAttribute : Attribute
    {
        private string methodName;

        public string MethodName { get { return methodName; } }

        public GodotMethodAttribute(string methodName)
        {
            this.methodName = methodName;
        }
    }
}
