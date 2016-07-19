package com.matteo.vc;

public interface Constant {

	public static final String PACKAGE = "com.matteo.vc";

	public static final String BROADCAST_ADDRESS = "255.255.255.255";

	public static final int UDP_BROADCAST_PORT = 18453;
	public static final int RTP_PORT = 6736;

	public interface Action {
		public static final String VC_SERVICE = "com.matteo.vc.service.VC_SERVICE";

		public static final String NEW_FRIEND = "com.matteo.vc.action.NEW_FRIEND";
		public static final String CALL_STATE = "com.matteo.vc.action.CALL_STATE";
	}

	public interface CallAction {
		public static final int ANSWER = 1;
		public static final int HANGUP = 2;
		public static final int REJECT = 3;
	}
	
	public interface CallState{
		public static final int OUTGO_FAILED=-1;
		public static final int INCOMING=1;
		public static final int ESTABLISHED=2;
		public static final int CONFIMRED=3;
		public static final int DISCONNECTED=4;
		public static final String KEY="call_state";
	}
}
