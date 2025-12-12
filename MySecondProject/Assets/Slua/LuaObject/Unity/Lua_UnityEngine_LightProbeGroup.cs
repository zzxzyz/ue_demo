using System;
using SLua;
using System.Collections.Generic;
using System.Reflection;
[UnityEngine.Scripting.Preserve]
public class Lua_UnityEngine_LightProbeGroup : LuaObject {
	[SLua.MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
	[UnityEngine.Scripting.Preserve]
	static public int get_probePositions(IntPtr l) {
		try {
			#if DEBUG
			var method = System.Reflection.MethodBase.GetCurrentMethod();
			string methodName = GetMethodName(method);
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.BeginSample(methodName);
			#else
			Profiler.BeginSample(methodName);
			#endif
			#endif
			UnityEngine.LightProbeGroup self=(UnityEngine.LightProbeGroup)checkSelf(l);
			pushValue(l,true);
			pushValue(l,self.probePositions);
			return 2;
		}
		catch(Exception e) {
			return error(l,e);
		}
		#if DEBUG
		finally {
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.EndSample();
			#else
			Profiler.EndSample();
			#endif
		}
		#endif
	}
	[SLua.MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
	[UnityEngine.Scripting.Preserve]
	static public int set_probePositions(IntPtr l) {
		try {
			#if DEBUG
			var method = System.Reflection.MethodBase.GetCurrentMethod();
			string methodName = GetMethodName(method);
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.BeginSample(methodName);
			#else
			Profiler.BeginSample(methodName);
			#endif
			#endif
			UnityEngine.LightProbeGroup self=(UnityEngine.LightProbeGroup)checkSelf(l);
			UnityEngine.Vector3[] v;
			checkArray(l,2,out v);
			// probePositions is read-only in Unity 2018.1+
			// Use reflection to check if setter exists
			var propInfo = typeof(UnityEngine.LightProbeGroup).GetProperty("probePositions");
			if (propInfo != null && propInfo.CanWrite) {
				propInfo.SetValue(self, v, null);
			}
			pushValue(l,true);
			return 1;
		}
		catch(Exception e) {
			return error(l,e);
		}
		#if DEBUG
		finally {
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.EndSample();
			#else
			Profiler.EndSample();
			#endif
		}
		#endif
	}
	[SLua.MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
	[UnityEngine.Scripting.Preserve]
	static public int get_dering(IntPtr l) {
		try {
			#if DEBUG
			var method = System.Reflection.MethodBase.GetCurrentMethod();
			string methodName = GetMethodName(method);
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.BeginSample(methodName);
			#else
			Profiler.BeginSample(methodName);
			#endif
			#endif
			UnityEngine.LightProbeGroup self=(UnityEngine.LightProbeGroup)checkSelf(l);
			// dering property does not exist in Unity 2018.4
			// Use reflection to safely check if property exists
			bool ret = false;
			var propInfo = typeof(UnityEngine.LightProbeGroup).GetProperty("dering");
			if (propInfo != null) {
				var value = propInfo.GetValue(self, null);
				if (value != null) {
					ret = (bool)value;
				}
			}
			pushValue(l,true);
			pushValue(l,ret);
			return 2;
		}
		catch(Exception e) {
			return error(l,e);
		}
		#if DEBUG
		finally {
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.EndSample();
			#else
			Profiler.EndSample();
			#endif
		}
		#endif
	}
	[SLua.MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
	[UnityEngine.Scripting.Preserve]
	static public int set_dering(IntPtr l) {
		try {
			#if DEBUG
			var method = System.Reflection.MethodBase.GetCurrentMethod();
			string methodName = GetMethodName(method);
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.BeginSample(methodName);
			#else
			Profiler.BeginSample(methodName);
			#endif
			#endif
			UnityEngine.LightProbeGroup self=(UnityEngine.LightProbeGroup)checkSelf(l);
			bool v;
			checkType(l,2,out v);
			// dering property does not exist in Unity 2018.4
			// Use reflection to safely check if property exists
			var propInfo = typeof(UnityEngine.LightProbeGroup).GetProperty("dering");
			if (propInfo != null && propInfo.CanWrite) {
				propInfo.SetValue(self, v, null);
			}
			pushValue(l,true);
			return 1;
		}
		catch(Exception e) {
			return error(l,e);
		}
		#if DEBUG
		finally {
			#if UNITY_5_5_OR_NEWER
			UnityEngine.Profiling.Profiler.EndSample();
			#else
			Profiler.EndSample();
			#endif
		}
		#endif
	}
	[UnityEngine.Scripting.Preserve]
	static public void reg(IntPtr l) {
		getTypeTable(l,"UnityEngine.LightProbeGroup");
		addMember(l,"probePositions",get_probePositions,set_probePositions,true);
		addMember(l,"dering",get_dering,set_dering,true);
		createTypeMetatable(l,null, typeof(UnityEngine.LightProbeGroup),typeof(UnityEngine.Behaviour));
	}
}
