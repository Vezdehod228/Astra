namespace ValveSDK
{
	struct surfacephysicsparams_t
	{
		// vphysics physical properties
		float			friction;
		float			elasticity;				// collision elasticity - used to compute coefficient of restitution
		float			density;				// physical density (in kg / m^3)
		float			thickness;				// material thickness if not solid (sheet materials) in inches
		float			dampening;
	};

	struct surfaceaudioparams_t
	{
		// sounds / audio data
		float			reflectivity;		// like elasticity, but how much sound should be reflected by this surface
		float			hardnessFactor;	// like elasticity, but only affects impact sound choices
		float			roughnessFactor;	// like friction, but only affects scrape sound choices

		// audio thresholds
		float			roughThreshold;	// surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
		float			hardThreshold;	// surface hardness > this causes "hard" impacts, < this causes "soft" impacts
		float			hardVelocityThreshold;	// collision velocity > this causes "hard" impacts, < this causes "soft" impacts
		// NOTE: Hard impacts must meet both hardnessFactor AND velocity thresholds
	};

	struct surfacesoundnames_t
	{
		unsigned short	stepleft;
		unsigned short	stepright;

		unsigned short	impactSoft;
		unsigned short	impactHard;

		unsigned short	scrapeSmooth;
		unsigned short	scrapeRough;

		unsigned short	bulletImpact;
		unsigned short	rolling;

		unsigned short	breakSound;
		unsigned short	strainSound;
	};

	struct surfacesoundhandles_t
	{
		short	stepleft;
		short	stepright;

		short	impactSoft;
		short	impactHard;

		short	scrapeSmooth;
		short	scrapeRough;

		short	bulletImpact;
		short	rolling;

		short	breakSound;
		short	strainSound;
	};

	struct surfacegameprops_t
	{
		// game movement data
		float			maxSpeedFactor;			// Modulates player max speed when walking on this surface
		float			jumpFactor;				// Indicates how much higher the player should jump when on the surface
		// Game-specific data
		unsigned short	material;

		// Indicates whether or not the player is on a ladder.
		unsigned char	climbable;
		unsigned char	pad;
	};

	struct surfacedata_t
	{
		surfacephysicsparams_t	physics;	// physics parameters
		surfaceaudioparams_t	audio;		// audio parameters
		surfacesoundnames_t		sounds;		// names of linked sounds
		surfacegameprops_t		game;		// Game data / properties

		surfacesoundhandles_t		soundhandles;
	};

	class IPhysicsSurfaceProps
	{
	public:
		surfacedata_t *GetSurfaceData(int surfaceDataIndex)
		{
			typedef surfacedata_t*(__thiscall* OriginalFn)(PVOID, int);
			return getvfunc<OriginalFn>(this, 5)(this, surfaceDataIndex);
		}
	};

	class CTrace
	{
	public:
		enum TraceType_t
		{
			TRACE_EVERYTHING = 0,
			TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
			TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
			TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
		};

		struct cplane_t
		{
			Vector normal;
			float dist;
			BYTE type;
			BYTE signBits;
			BYTE pad[2];
		};

		struct csurface_t
		{
			const char*		name;
			short			surfaceProps;
			unsigned short	flags;
		};

		struct Ray_t
		{
			Vector m_Start;
			Vector m_Delta;
			Vector m_StartOffset;
			Vector m_Extents;

			bool	m_IsRay;
			bool	m_IsSwept;

			void Init(const Vector& start, const Vector& end)
			{
				m_Delta = end - start;

				m_IsSwept = (m_Delta.LengthSqr() != 0);

				m_Extents = Vector(0, 0, 0);
				m_IsRay = true;

				m_StartOffset = Vector(0, 0, 0);
				m_Start = start;
			}

			void Init(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs)
			{
				m_Delta = end - start;

				m_IsSwept = (m_Delta.LengthSqr() != 0);

				m_Extents = maxs - mins;
				m_Extents *= 0.5f;
				m_IsRay = (m_Extents.LengthSqr() < 1e-6);

				m_StartOffset = mins + maxs;
				m_StartOffset *= 0.5f;
				m_Start = start + m_StartOffset;
				m_StartOffset *= -1.0f;
			}
		};

		class CBaseTrace
		{
		public:
			Vector			startpos;
			Vector			endpos;
			cplane_t		plane;
			float			fraction;
			int				contents;
			unsigned short	dispFlags;
			bool			allsolid;
			bool			startsolid;
		};

		class CGameTrace : public CBaseTrace
		{
		public:
			float			fractionleftsolid;
			csurface_t		surface;
			int				hitgroup;
			short			physicsbone;
			CBaseEntity*	m_pEnt;
			int				hitbox;
		};

		typedef CGameTrace trace_t;
		class CBaseHandle;
		class IHandleEntity
		{
		public:
			virtual ~IHandleEntity() {}
			virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
			virtual const CBaseHandle& GetRefEHandle() const = 0;
		};

		class ITraceFilter
		{
		public:
			virtual bool ShouldHitEntity( IHandleEntity *pEntity, int contentsMask ) = 0;
			virtual TraceType_t	GetTraceType() const = 0;
		};

		class CTraceFilter : public ITraceFilter
		{
		public:
			virtual TraceType_t	GetTraceType() const
			{
				return TRACE_EVERYTHING;
			}
		};

		class CSimpleTraceFilter : public CTraceFilter
		{
		public:
			CSimpleTraceFilter(PVOID pEnt)
			{
				m_pPassEnt = pEnt;
			}

			virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
			{
				return !(pHandleEntity == m_pPassEnt);
			}

			PVOID m_pPassEnt;
		};

		class CTraceFilterSkipTwoEnts : public CTraceFilter
		{
		public:
			CTraceFilterSkipTwoEnts(PVOID pEnt,PVOID pEnt2)
			{
				m_pPassEnt = pEnt;
				m_pPassEnt2 = pEnt2;
			}

			virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
			{
				return !(pHandleEntity == m_pPassEnt || pHandleEntity == m_pPassEnt2);
			}

			PVOID m_pPassEnt;
			PVOID m_pPassEnt2;
		};

		class IClientUnknown : public IHandleEntity
		{
		public:
			virtual PVOID GetCollideable() = 0;
			virtual PVOID GetClientNetworkable() = 0;
			virtual PVOID GetClientRenderable() = 0;
			virtual PVOID GetIClientEntity() = 0;
			virtual CBaseEntity *GetBaseEntity() = 0;
			virtual PVOID GetClientThinkable() = 0;
			virtual void *class1(void) = 0;
			virtual void *class2(void) = 0;
		};

		class CTraceFilterNoPlayer : public CTraceFilter
		{
		public:
			CTraceFilterNoPlayer() {}
			virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
			{
				IClientUnknown *pUnk = (IClientUnknown*)pServerEntity;
				CBaseEntity *pEnt = pUnk->GetBaseEntity();
				return !pEnt->IsPlayer();
			}
		};

		int GetPointContents( const Vector &vecAbsPosition, IHandleEntity** ppEntity = NULL )
		{
			typedef int(__thiscall* OriginalFn)(PVOID, const Vector &, IHandleEntity**);
			return getvfunc<OriginalFn>(this, 0)(this, vecAbsPosition, ppEntity);
		}

		void TraceRay(const Ray_t &ray, unsigned int flMask, ITraceFilter *pTraceFilter, trace_t *ptrace)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, const Ray_t &, unsigned int, ITraceFilter *, trace_t *);
			getvfunc<OriginalFn>(this, 4)(this, ray, flMask, pTraceFilter, ptrace);
		}
	};
}